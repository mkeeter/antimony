#include <boost/python.hpp>

#include "fab.h"
#include "shape.h"
#include "transform.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(fab)
{
    class_<Bounds>("Bounds", init<>())
            .def(init<float, float, float, float>())
            .def(init<float, float, float, float, float, float>())
            .def_readwrite("xmin", &Bounds::xmin)
            .def_readwrite("ymin", &Bounds::ymin)
            .def_readwrite("zmin", &Bounds::zmin)
            .def_readwrite("xmax", &Bounds::xmax)
            .def_readwrite("ymax", &Bounds::ymax)
            .def_readwrite("zmax", &Bounds::zmax);

    class_<MathShape>("MathShape", init<std::string>())
            .def(init<std::string, float, float, float, float>())
            .def(init<std::string, float, float, float, float, float, float>())
            .def(init<std::string, Bounds>())
            .def_readwrite("math", &MathShape::math)
            .def_readwrite("bounds", &MathShape::bounds)
            .def("map", &MathShape::map);


    class_<Transform>("Transform",
        init<std::string, std::string, std::string, std::string>())
            .def(init<std::string, std::string, std::string,
                      std::string, std::string, std::string>())
            .def_readwrite("x_forward", &Transform::x_forward)
            .def_readwrite("y_forward", &Transform::y_forward)
            .def_readwrite("z_forward", &Transform::z_forward)
            .def_readwrite("z_reverse", &Transform::z_reverse)
            .def_readwrite("y_reverse", &Transform::y_reverse)
            .def_readwrite("z_reverse", &Transform::z_reverse);
}

void fab::loadModule()
{
    PyImport_AppendInittab("fab", PyInit_fab);
}
