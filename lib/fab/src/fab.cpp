#include <boost/python.hpp>

#include "fab/fab.h"
#include "fab/types/shape.h"
#include "fab/types/transform.h"

using namespace boost::python;

void fab::onParseError(const fab::ParseError &e)
{
    (void)e;
    PyErr_SetString(PyExc_RuntimeError, "Failed to parse math expression");
}

void fab::onShapeError(const fab::ShapeError& e)
{
    (void)e;
    PyErr_SetString(PyExc_RuntimeError, "Could not construct Shape object.");
}

BOOST_PYTHON_MODULE(_fabtypes)
{
    class_<Bounds>("Bounds", init<>())
            .def(init<>())
            .def(init<float, float, float, float>())
            .def(init<float, float, float, float, float, float>())
            .def_readonly("xmin", &Bounds::xmin)
            .def_readonly("ymin", &Bounds::ymin)
            .def_readonly("zmin", &Bounds::zmin)
            .def_readonly("xmax", &Bounds::xmax)
            .def_readonly("ymax", &Bounds::ymax)
            .def_readonly("zmax", &Bounds::zmax)
            .def("is_bounded_xy", &Bounds::is_bounded_xy,
                 "Returns True if both x and y bounds are non-infinite.")
            .def("is_bounded_xyz", &Bounds::is_bounded_xyz,
                 "Returns True if both all bounds are non-infinite.");

    class_<Shape>("Shape", init<>())
            .def(init<std::string>())
            .def(init<std::string, float, float, float, float>())
            .def(init<std::string, float, float, float, float, float, float>())
            .def(init<std::string, Bounds>())
            .def(init<object>())
            .def_readonly("math", &Shape::math)
            .def_readonly("bounds", &Shape::bounds)
            .def_readwrite("_r", &Shape::r)
            .def_readwrite("_g", &Shape::g)
            .def_readwrite("_b", &Shape::b)
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
    register_exception_translator<fab::ShapeError>(fab::onShapeError);
}


PyTypeObject* fab::ShapeType = NULL;

void fab::preInit()
{
    PyImport_AppendInittab("_fabtypes", PyInit__fabtypes);
}

void fab::postInit(const char* path_dir)
{
    PyObject* fabtypes = PyImport_ImportModule("_fabtypes");
    ShapeType = (PyTypeObject*)PyObject_GetAttrString(fabtypes, "Shape");

    // Modify the default search path to include the application's directory
    // (as this doesn't happen on Linux by default)
    PyList_Insert(PySys_GetObject("path"), 0, PyUnicode_FromString(path_dir));
}
