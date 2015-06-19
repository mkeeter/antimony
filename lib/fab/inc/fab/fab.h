#ifndef FAB_H
#define FAB_H

#include <Python.h>

namespace fab
{
    struct ParseError {};
    struct ShapeError {};

    /** Callback that raises a Python runtime exception. */
    void onParseError(ParseError const& e);

    /** Callback that raises a Python runtime exception. */
    void onShapeError(ShapeError const& e);

    /** Loads the fab module in Python's namespace.
     *
     *  Must be called before Py_Initialize().
     */
    void preInit();

    /** Loads the fab module in Python's namespace.
     *
     *  path_dir is added to Python's search path.
     */
    void postInit(const char* path_dir);

    extern PyTypeObject* ShapeType;
}

#endif // FAB_H
