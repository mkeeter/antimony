#ifndef UI_HOOKS
#define UI_HOOKS

#include <boost/python.hpp>
#include <QString>
#include <QVector3D>

class ViewportScene;
class Node;

struct ScriptUIHooks
{
    ScriptUIHooks() : scene(NULL) {}
    static long getInstruction();
    static boost::python::object point(boost::python::tuple args,
                                       boost::python::dict kwargs);

    /*
     *  Extracts as tuple or list to a list of the given object type.
     *  Throws a HookException if any single extract() call fails.
     */
    template <typename T, typename O>
    static QVector<T> _extractList(O obj);

    /*
     *  Extracts a tuple or list to a list of the given object type.
     *  Throws a HookException if the input is not a list or tuple, or if
     *  any single extraction fails.
     */
    template <typename T>
    static QVector<T> extractList(boost::python::object obj);

    /*
     *  Converts a (list|tuple) of 3-element (lists|tuples) to vectors.
     *  Throws a HookException on failure.
     */
    static QVector<QVector3D> extractVectors(boost::python::object obj);

    /*
     *  Returns the datum name with value == obj
     *  (or the empty string if no match is found).
     */
    QString getDatum(PyObject* obj);

    Node* node;
    ViewportScene* scene;
};

#endif
