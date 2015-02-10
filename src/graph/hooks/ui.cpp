#include "graph/hooks/ui.h"

#include "ui/viewport/viewport_scene.h"

#include "control/point.h"

using namespace boost::python;

object ScriptUIHooks::point(tuple args, dict kwargs)
{
    ScriptUIHooks& self = extract<ScriptUIHooks&>(args[0])();

    // Get the current bytecode instruction
    // (used to uniquely identify calls to this function)
    auto inspect_module = PyImport_ImportModule("inspect");
    auto frame = PyObject_CallMethod(inspect_module, "currentframe", NULL);
    auto f_lasti = PyObject_GetAttrString(frame, "f_lasti");
    long lasti = PyLong_AsLong(f_lasti);
    Q_ASSERT(!PyErr_Occurred());

    // Clean up these objects immediately
    for (auto o : {inspect_module, frame, f_lasti})
        Py_DECREF(o);

    // Find a Control if it already exists.
    Control* c = self.scene->getControl(self.node, lasti);
    if (!c)
    {
        c = new ControlPoint(self.node);
        self.scene->registerControl(c);
    }

    auto p = dynamic_cast<ControlPoint*>(c);
    Q_ASSERT(p);
    p->update(1, 1, 1, 2, QColor(1.0, 0, 0));

    // Return None
    return object();
}
