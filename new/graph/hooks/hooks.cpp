#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/title.h"
#include "graph/proxy/graph.h"

#include "graph/script_node.h"

using namespace boost::python;

////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(_AppHooks)
{
    class_<ScriptTitleHook>("ScriptTitleHook", init<>())
        .def("__call__", &ScriptTitleHook::call);

    register_exception_translator<AppHooks::Exception>(
            AppHooks::onException);
}

////////////////////////////////////////////////////////////////////////////////

void AppHooks::onException(const AppHooks::Exception& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

void AppHooks::preInit()
{
    PyImport_AppendInittab("_AppHooks", PyInit__AppHooks);
}

////////////////////////////////////////////////////////////////////////////////

void AppHooks::loadScriptHooks(PyObject* g, ScriptNode* n)
{
    // Lazy initialization of hooks module
    static PyObject* hooks_module = NULL;
    if (hooks_module == NULL)
        hooks_module = PyImport_ImportModule("_AppHooks");

    // Lazy initialization of named tuple constructor
    static PyObject* sb_tuple = NULL;
    if (sb_tuple == NULL)
    {
        PyObject* collections = PyImport_ImportModule("collections");
        sb_tuple = PyObject_CallMethod(
                collections, "namedtuple", "(s[sss])", "SbObject",
                "ui", "export", "color");
        Py_DECREF(collections);
    }

    auto title_func = PyObject_CallMethod(
            hooks_module, "ScriptTitleHook", NULL);
    Q_ASSERT(!PyErr_Occurred());

    auto title_ref = extract<ScriptTitleHook*>(title_func)();
    title_ref->proxy = proxy->getNodeProxy(n);
    PyDict_SetItemString(g, "title", title_func);
}

void AppHooks::loadDatumHooks(PyObject* g)
{
    PyDict_SetItemString(g, "fab", PyImport_ImportModule("fab"));
    PyDict_SetItemString(g, "math", PyImport_ImportModule("math"));
    Q_ASSERT(!PyErr_Occurred());
}
