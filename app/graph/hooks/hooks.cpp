#include <boost/python.hpp>
#include <boost/python/raw_function.hpp>

#include "graph/hooks/hooks.h"
#include "graph/hooks/title.h"
#include "graph/hooks/export.h"
#include "graph/hooks/ui.h"

#include "graph/proxy/graph.h"
#include "graph/proxy/node.h"

#include "graph/script_node.h"

#include "app/colors.h"

using namespace boost::python;

////////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(_AppHooks)
{
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

    class_<ScriptExportHooks>("ScriptExportHooks", init<>())
        .def("stl", raw_function(&ScriptExportHooks::stl),
                "stl(shape, bounds=None, pad=True, filename=None,\n"
                "    resolution=None, detect_features=False)\n"
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
        .def("heightmap", raw_function(&ScriptExportHooks::heightmap),
                "heightmap(shape, bounds=None, pad=True, filename=None,\n"
                "          resolution=None, mm_per_unit=25.4)\n"
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

    {   // Create title callback
        auto title_func = PyObject_CallMethod(
                hooks_module, "ScriptTitleHook", NULL);
        Q_ASSERT(!PyErr_Occurred());

        auto title_ref = extract<ScriptTitleHook*>(title_func)();
        title_ref->proxy = proxy->getNodeProxy(n);
        PyDict_SetItemString(g, "title", title_func);
    }

    PyObject* ui_obj = NULL;
    {   // Create UI object
        ui_obj = PyObject_CallMethod(
                hooks_module, "ScriptUIHooks", NULL);
        auto ui_ref = extract<ScriptUIHooks*>(ui_obj)();
        ui_ref->proxy = proxy->getNodeProxy(n);
        ui_ref->proxy->clearControlTouched();
    }

    PyObject* export_obj = NULL;
    {   // Create export object
        export_obj = PyObject_CallMethod(
                hooks_module, "ScriptExportHooks", NULL);
        auto export_ref = extract<ScriptExportHooks*>(export_obj)();
        export_ref->node = n;
        export_ref->proxy = proxy->getNodeProxy(n);
        export_ref->proxy->clearExportWorker();
    }

    {   // Pack export, ui, and colors into the 'sb' tuple
        PyObject* sb = PyObject_CallFunctionObjArgs(
                sb_tuple, ui_obj, export_obj, Colors::PyColors(), NULL);
        PyDict_SetItemString(g, "sb", sb);
    }
}

void AppHooks::loadDatumHooks(PyObject* g)
{
    PyDict_SetItemString(g, "fab", PyImport_ImportModule("fab"));
    PyDict_SetItemString(g, "math", PyImport_ImportModule("math"));
    Q_ASSERT(!PyErr_Occurred());
}
