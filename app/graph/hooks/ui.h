#pragma once

#include <boost/python.hpp>

#include <QColor>
#include <QSet>
#include <QVector3D>

class NodeProxy;

struct ScriptUIHooks
{
    /*
     *  Returns the line number at which this function was called.
     *  Throws HookException if the line number has been seen already.
     */
    long getInstruction();

    /*
     *  Returns a drag function from a kwargs dictionary
     *
     *  If kwargs['drag'] is a tuple, construct from tuple;
     *  otherwise, assume kwargs['drag'] is callable and use it.
     */
    PyObject* getDragFunction(boost::python::dict kwargs);

    /*
     *  Generates a drag function from the given (x, y, z) value tuple
     *  (by matching against datum values)
     */
    PyObject* tupleDragFunction(boost::python::tuple t);

    /*
     *  Static helper functions that extract 'this' and construct a Control
     */
    static boost::python::object point(boost::python::tuple args,
                                       boost::python::dict kwargs);
    static boost::python::object wireframe(boost::python::tuple args,
                                       boost::python::dict kwargs);

    /*
     *  Returns a datum (by name) with current value that equals obj
     *  (or the empty string if none is found)
     */
    QString getDatum(PyObject* obj);

    /*  Set of instructions used to uniquely identify Controls  */
    QSet<long> instructions;

    /*  Pointer to proxy node  */
    NodeProxy* proxy=nullptr;
};
