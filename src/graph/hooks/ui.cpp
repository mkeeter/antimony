#include "graph/hooks/ui.h"
#include "graph/hooks/hooks.h"

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
        self.scene->registerControl(self.node, lasti, c);
    }

    auto p = dynamic_cast<ControlPoint*>(c);
    Q_ASSERT(p);

    if (len(args) != 4)
        throw hooks::HookException("Expected x, y, z as arguments");

    // Extract x, y, z as floats from first three arguments.
    extract<float> x_(args[1]);
    extract<float> y_(args[2]);
    extract<float> z_(args[3]);
    if (!x_.check())
        throw hooks::HookException("x value must be a number");
    if (!y_.check())
        throw hooks::HookException("y value must be a number");
    if (!z_.check())
        throw hooks::HookException("z value must be a number");
    float x = x_();
    float y = y_();
    float z = z_();

    p->update(x, y, z, 20, QColor(255, 255, 0));

    // Return None
    return object();
}
