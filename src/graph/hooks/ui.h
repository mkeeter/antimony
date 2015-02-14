#ifndef UI_HOOKS
#define UI_HOOKS

#include <boost/python.hpp>
#include <QString>

class ViewportScene;
class Node;

struct ScriptUIHooks
{
    ScriptUIHooks() : scene(NULL) {}
    static long getInstruction();
    static boost::python::object point(boost::python::tuple args,
                                       boost::python::dict kwargs);

    /*
     *  Returns the datum name with value == obj
     *  (or the empty string if no match is found).
     */
    QString getDatum(PyObject* obj);

    Node* node;
    ViewportScene* scene;
};

#endif
