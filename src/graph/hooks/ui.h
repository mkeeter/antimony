#ifndef UI_HOOKS
#define UI_HOOKS

#include <boost/python.hpp>

#include <QString>
#include <QVector3D>
#include <QSet>

class ViewportScene;
class Node;

struct ScriptUIHooks
{
    ScriptUIHooks() : scene(NULL) {}

    /*
     *  Returns the line number at which this function was called.
     *  Throws HookException if the line number has been seen already.
     */
    long getInstruction();

    static boost::python::object point(boost::python::tuple args,
                                       boost::python::dict kwargs);
    static boost::python::object wireframe(boost::python::tuple args,
                                       boost::python::dict kwargs);

    /*
     *  Tries to extract an RGB color tuple from kwargs.
     *  Returns color if no such argument exists;
     *  throws a hooks::HookException if the argument exists but is
     *  incorrectly constructed.
     */
    static QColor getColor(QColor color, boost::python::dict kwargs);

    /*
     *  Tries to get a particular value from kwargs.
     *  Returns v if no such value exists;
     *  throws hooks::HookException if the value is there but not a float.
     */
    static float getFloat(float v, boost::python::dict kwargs,
                          std::string key);

    /*
     *  Tries to extract a boolean value from kwargs.
     *  Returns b if no value with that key exists;
     *  throws hooks::HookException if the value is there but not a bool.
     */
    static bool getBool(bool b, boost::python::dict kwargs,
                        std::string key);

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

    QSet<long> instructions;

    Node* node;
    ViewportScene* scene;
};

#endif
