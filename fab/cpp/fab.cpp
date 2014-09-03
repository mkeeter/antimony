#include <boost/python.hpp>

#include <QFile>
#include <QString>
#include <QTextStream>

#include "cpp/fab.h"
#include "cpp/shape.h"
#include "cpp/transform.h"

using namespace boost::python;

void fab::onParseError(const fab::ParseError &e)
{
    (void)e;
    PyErr_SetString(PyExc_RuntimeError, "Failed to parse math expression");
}

BOOST_PYTHON_MODULE(fab)
{
    class_<Bounds>("Bounds", init<>())
            .def(init<float, float, float, float>())
            .def(init<float, float, float, float, float, float>())
            .def_readonly("xmin", &Bounds::xmin)
            .def_readonly("ymin", &Bounds::ymin)
            .def_readonly("zmin", &Bounds::zmin)
            .def_readonly("xmax", &Bounds::xmax)
            .def_readonly("ymax", &Bounds::ymax)
            .def_readonly("zmax", &Bounds::zmax);

    class_<Shape>("Shape", init<std::string>())
            .def(init<std::string, float, float, float, float>())
            .def(init<std::string, float, float, float, float, float, float>())
            .def(init<std::string, Bounds>())
            .def_readonly("math", &Shape::math)
            .def_readonly("bounds", &Shape::bounds)
            .def("map", &Shape::map)
            .def(self & self)
            .def(self | self)
            .def(~self);


    class_<Transform>("Transform",
        init<std::string, std::string, std::string, std::string>())
            .def(init<std::string, std::string, std::string,
                      std::string, std::string, std::string>())
            .def_readonly("x_forward", &Transform::x_forward)
            .def_readonly("y_forward", &Transform::y_forward)
            .def_readonly("z_forward", &Transform::z_forward)
            .def_readonly("z_reverse", &Transform::z_reverse)
            .def_readonly("y_reverse", &Transform::y_reverse)
            .def_readonly("z_reverse", &Transform::z_reverse);

    register_exception_translator<fab::ParseError>(fab::onParseError);
}


PyTypeObject* fab::ShapeType = NULL;

void fab::preInit()
{
    PyImport_AppendInittab("fab", PyInit_fab);
}

void fab::postInit()
{
    PyObject* shapes_module = PyModule_New("shapes");

    PyObject* fab = PyImport_ImportModule("fab");

    ShapeType = (PyTypeObject*)PyObject_GetAttrString(fab, "Shape");

    QFile shapes_file(":fab/py/shapes.py");
    shapes_file.open(QFile::ReadOnly | QFile::Text);
    QString shapes_txt = QTextStream(&shapes_file).readAll();

    // Evaluate shapes.py, using the shapes module as the globals dict
    PyObject* dict = PyModule_GetDict(shapes_module);
    PyDict_SetItemString(dict, "__builtins__", PyEval_GetBuiltins());
    PyRun_String(shapes_txt.toStdString().c_str(), Py_file_input,
                 dict, dict);
    assert(PyErr_Occurred() == NULL);

    PyObject_SetAttrString(fab, "shapes", shapes_module);

    Py_DECREF(fab);
}
