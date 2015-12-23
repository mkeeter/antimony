#include <Python.h>

#include "graph/script.h"
#include "graph/script_node.h"
#include "graph/util.h"
#include "graph/proxy.h"
#include "graph/hooks/hooks.h"
#include "graph/hooks/external.h"

Script::Script(ScriptNode* parent)
    : error_lineno(-1), parent(parent)
{
    // Nothing to do here
}

void Script::update()
{
    // Reset the source list (which we'll populate at the end of evaluation)
    sources.clear();
    sources.insert(this);

    const auto old_active = parent->childDatums();
    active.clear();
    error_lineno = -1;
    error.clear();

    globals = Py_BuildValue(
            "{sO}", "__builtins__", PyEval_GetBuiltins());
    Hooks::load(globals, parent);
    parent->loadScriptHooks(globals);

    // Swap in a stringIO object for stdout, saving stdout in out
    PyObject* sys_mod = PyImport_ImportModule("sys");
    PyObject* io_mod = PyImport_ImportModule("io");
    PyObject* stdout_obj = PyObject_GetAttrString(sys_mod, "stdout");
    PyObject* stderr_obj = PyObject_GetAttrString(sys_mod, "stderr");
    PyObject* string_out = PyObject_CallMethod(io_mod, "StringIO", NULL);
    PyObject_SetAttrString(sys_mod, "stdout", string_out);
    PyObject_SetAttrString(sys_mod, "stderr", string_out);

    // Run the script
    PyObject* out = PyRun_String(
            script.c_str(), Py_file_input, globals, globals);
    Py_XDECREF(out);

    if (PyErr_Occurred())
    {
        auto err = getPyError();
        error = err.first;
        error_lineno = err.second;
        PyErr_Clear();
    }

    Py_DECREF(globals);
    globals = NULL;

    // Get the output from the StringIO object
    PyObject* s = PyObject_CallMethod(string_out, "getvalue", NULL);
    output = std::string(PyUnicode_AsUTF8(s));

    // Swap stdout back into sys.stdout
    PyObject_SetAttrString(sys_mod, "stdout", stdout_obj);
    PyObject_SetAttrString(sys_mod, "stderr", stderr_obj);
    for (auto o : {sys_mod, io_mod, stdout_obj, stderr_obj, string_out, s})
        Py_DECREF(o);

    // Filter out default arguments to input datums, to make the script
    // simpler to read (because input('x', float, 12.0f) looks odd when
    // x doesn't have a value of 12 anymore).
    if (script != prev_script)
    {
        std::regex input("(.*input\\([^(),]+,[^(),]+),[^(),]+(\\).*)");
        script = std::regex_replace(script, input, std::string("$1$2"));
    }
    prev_script = script;

    // If the script evaluation failed, recover the old set of active datums.
    // (so that we don't delete datums on script errors)
    if (error_lineno != -1)
        active.insert(old_active.begin(), old_active.end());

    // Populate the script's source array with all its input datums
    parent->update(active);
    for (const auto& d : parent->datums)
        if (!d->isOutput())
            sources.insert(d->sources.begin(), d->sources.end());

    // Then make all of the output datums depend on the script and all
    // of its (newly-populated) sources.
    for (const auto& d : parent->datums)
        if (d->isOutput())
            d->sources.insert(sources.begin(), sources.end());

    // Finally, update anything that's registered itself as a watcher
    triggerWatchers();
}

ScriptState Script::getState() const
{
    return (ScriptState){script, error, output, error_lineno};
}

void Script::inject(std::string name, PyObject* value)
{
    assert(globals != NULL);
    PyDict_SetItemString(globals, name.c_str(), value);
}

void Script::setText(std::string t)
{
    script = t;
    trigger();
}
