#include <boost/python.hpp>

#include "fab.h"
#include "shape.h"

using namespace boost::python;

BOOST_PYTHON_MODULE(fab)
{
    class_<MathShape>("MathShape", init<std::string, float, float, float, float, float, float>())
            .def_readwrite("math", &MathShape::math)
            .def_readwrite("xmin", &MathShape::xmin)
            .def_readwrite("ymin", &MathShape::ymin)
            .def_readwrite("zmin", &MathShape::zmin)
            .def_readwrite("xmax", &MathShape::xmax)
            .def_readwrite("ymax", &MathShape::ymax)
            .def_readwrite("zmax", &MathShape::zmax);
}

void fab::loadModule()
{
    PyImport_AppendInittab("fab", PyInit_fab);
}
