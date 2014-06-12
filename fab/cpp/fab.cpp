#include <boost/python.hpp>

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

    class_<MathShape>("MathShape", init<std::string>())
            .def(init<std::string, float, float, float, float>())
            .def(init<std::string, float, float, float, float, float, float>())
            .def(init<std::string, Bounds>())
            .def_readonly("math", &MathShape::math)
            .def_readonly("bounds", &MathShape::bounds)
            .def("map", &MathShape::map);


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

void fab::loadModule()
{
    PyImport_AppendInittab("fab", PyInit_fab);
}
