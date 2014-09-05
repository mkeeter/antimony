#ifndef FAB_H
#define FAB_H

#include <Python.h>

namespace fab
{
    struct ParseError {};

    /** Callback that raises a Python runtime exception. */
    void onParseError(ParseError const& e);

    /** Loads the fab module in Python's namespace.
     *
     *  Must be called before Py_Initialize().
     */
    void preInit();

    /** Loads the fab module in Python's namespace.
     *
     *  Must be called after Py_Initialize() and the application
     *  is created (as it uses QCoreApplication to set the working
     *  directory).
     */
    void postInit();

    extern PyTypeObject* ShapeType;
}

#endif // FAB_H
