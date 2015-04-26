#include <Python.h>

#include "control/root.h"
#include "control/control.h"
#include "control/proxy.h"

ControlRoot::ControlRoot(Node* n)
    : node(n)
{
    // Nothing to do here
}

void ControlRoot::registerControl(long index, Control* c)
{
    controls[index] = c;
}

Control* ControlRoot::get(long index) const
{
    if (!controls.contains(index) || controls[index].isNull())
        return NULL;
    return controls[index];
}

void ControlRoot::makeProxiesFor(Viewport* v)
{
    prune();

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        new ControlProxy(itr.value(), v);
}

void ControlRoot::prune()
{
    decltype(controls) new_controls;
    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        if (!itr.value().isNull())
            new_controls[itr.key()] = itr.value();
    controls = new_controls;
}
