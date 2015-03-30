#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/input.h"
#include "graph/hooks/output.h"
#include "graph/hooks/title.h"
#include "graph/hooks/ui.h"
#include "graph/hooks/meta.h"

#include "graph/datum/datums/script_datum.h"
#include "graph/node/node.h"

#include "app/app.h"

// Unfortunate coupling of core UI code, but this is
// necessary to properly set up the export button hooks.
#include "ui/canvas/graph_scene.h"
#include "ui/canvas/inspector/inspector_buttons.h"
#include "ui/canvas/inspector/inspector.h"

using namespace boost::python;

void hooks::onHookException(const hooks::HookException& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.message.c_str());
}

BOOST_PYTHON_MODULE(_hooks)
{
    class_<ScriptInputHook>("ScriptInputHook", init<>())
        .def("__call__", &ScriptInputHook::call)
        .def("__call__", &ScriptInputHook::call_with_default);

    class_<ScriptOutputHook>("ScriptOutputHook", init<>())
        .def("__call__", &ScriptOutputHook::call);

    class_<ScriptTitleHook>("ScriptTitleHook", init<>())
        .def("__call__", &ScriptTitleHook::call);

    class_<ScriptUIHooks>("ScriptUIHooks", init<>())
        .def("point", raw_function(&ScriptUIHooks::point),
                "point(x, y, z, r=5, color=(150, 150, 255), drag=None)\n"
                "    Constructs a point in the 3D viewport\n"
                "    x, y, z are the point's position\n\n"
                "    Valid kwargs:\n"
                "    r sets the point's radius\n"
                "    color sets the color as a 3-tuple (0-255)\n"
                "    drag is an optional callback:\n"
                "      It is invoked as drag(this, x, y, z)\n"
                "      where this is a representation of the parent\n"
                "      object and x, y, z are the position to which\n"
                "      the point has been dragged."
                )
        .def("wireframe", raw_function(&ScriptUIHooks::wireframe),
                "wireframe(pts, t=3, color=(150, 150, 255), close=false)\n"
                "    Constructs a wireframe in the 3D viewport\n"
                "    pts is a list of 3-float tuples\n\n"
                "    Valid kwargs:\n"
                "    t sets the line's thickness\n"
                "    color sets the color as a 3-tuple (0-255)\n"
                "    close makes the loop closed"
                );

    class_<ScriptMetaHooks>("ScriptMetaHooks", init<>())
        .def("export_stl", raw_function(&ScriptMetaHooks::export_stl),
                "export_stl(shape, bounds=None, pad=True, filename=None,\n"
                "           resolution=None, detect_features=False)\n"
                "    Registers a .stl exporter for the given shape.\n"
                "    Valid kwargs:\n"
                "    bounds is either a fab.types.Bounds object or None.\n"
                "      If it is None, bounds are taken from the shape.\n"
                "    pad sets whether bounds should be padded a small amount\n"
                "      (to prevent edge conditions at the models' edges)\n"
                "    filename sets the filename.\n"
                "      If None, a dialog will open to select a file.\n"
                "    resolution sets the resolution.\n"
                "      If None, a dialog will open to select the resolution.\n"
                "    detect_features enables feature detection (experimental)"
                )
        .def("export_heightmap", raw_function(&ScriptMetaHooks::export_heightmap),
                "export_heightmap(shape, bounds=None, pad=True, filename=None,\n"
                "                 resolution=None, mm_per_unit=25.4)\n"
                "    Registers a .stl exporter for the given shape.\n"
                "    Valid kwargs:\n"
                "    bounds is either a fab.types.Bounds object or None.\n"
                "      If it is None, bounds are taken from the shape.\n"
                "    pad sets whether bounds should be padded a small amount\n"
                "      (to prevent edge conditions at the models' edges)\n"
                "    filename sets the filename.\n"
                "      If None, a dialog will open to select a file.\n"
                "    resolution sets the resolution.\n"
                "      If None, a dialog will open to select the resolution.\n"
                "    mm_per_unit maps Antimony to real-world units."
                );

    register_exception_translator<hooks::HookException>(
            hooks::onHookException);
}

// Lazy initialization of the hooks module.
static PyObject* _hooks_module = NULL;

void hooks::preInit()
{
    PyImport_AppendInittab("_hooks", PyInit__hooks);
}

PyObject* hooks::loadHooks(PyObject* g, ScriptDatum* d)
{
    if (_hooks_module == NULL)
        _hooks_module = PyImport_ImportModule("_hooks");

    auto input_func = PyObject_CallMethod(
            _hooks_module, "ScriptInputHook", NULL);
    auto output_func = PyObject_CallMethod(
            _hooks_module, "ScriptOutputHook", NULL);
    auto title_func = PyObject_CallMethod(
            _hooks_module, "ScriptTitleHook", NULL);
    auto ui_obj = PyObject_CallMethod(
            _hooks_module, "ScriptUIHooks", NULL);
    auto meta_obj = PyObject_CallMethod(
            _hooks_module, "ScriptMetaHooks", NULL);

    extract<ScriptInputHook&>(input_func)().datum = d;
    extract<ScriptOutputHook&>(output_func)().datum = d;
    extract<ScriptTitleHook&>(title_func)().datum = d;

    auto node = static_cast<Node*>(d->parent());
    extract<ScriptUIHooks&>(ui_obj)().scene = App::instance()->getViewScene();
    extract<ScriptUIHooks&>(ui_obj)().node = node;

    // Hook the 'meta' object to the relevant NodeInspector's output button.
    auto inspector = App::instance()->getGraphScene()->getInspector(node);
    if (inspector)
    {
        auto export_button = inspector->getButton<InspectorExportButton>();
        export_button->clearWorker();
        extract<ScriptMetaHooks&>(meta_obj)().button = export_button;
    }

    PyDict_SetItemString(g, "input", input_func);
    PyDict_SetItemString(g, "output", output_func);
    PyDict_SetItemString(g, "title", title_func);
    PyDict_SetItemString(g, "meta", meta_obj);

    auto fab = PyImport_ImportModule("fab");
    PyObject* old_ui = NULL;
    if (PyObject_HasAttrString(fab, "ui"))
        old_ui = PyObject_GetAttrString(fab, "ui");

    PyObject_SetAttrString(fab, "ui", ui_obj);
    Py_DECREF(fab);
    Py_DECREF(ui_obj);

    return old_ui;
}
