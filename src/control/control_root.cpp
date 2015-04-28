#include <Python.h>

#include "control/control_root.h"
#include "control/control.h"
#include "control/proxy.h"

#include "ui/viewport/viewport.h"

ControlRoot::ControlRoot(Node* n)
    : node(n), selected(false)
{
    // Nothing to do here
}

void ControlRoot::registerControl(long index, Control* c)
{
    controls[index] = c;
    connect(c, &Control::proxySelectionChanged,
            this, &ControlRoot::changeProxySelection);
    connect(c, &Control::proxySelectionChanged,
            [=](bool s){ this->selected = s; });
    connect(this, &ControlRoot::changeProxySelection,
            c, &Control::changeProxySelection);
}

Control* ControlRoot::get(long index) const
{
    if (!controls.contains(index) || controls[index].isNull())
        return NULL;
    return controls[index];
}

void ControlRoot::setGlow(bool g)
{
    for (auto itr=controls.begin(); itr != controls.end(); ++itr)
        if (!itr.value().isNull())
            itr.value()->setGlow(g);
}

void ControlRoot::makeProxiesFor(Viewport* v)
{
    prune();

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
    {
        auto p = new ControlProxy(itr.value(), v);
        if (selected)
            p->selectProxy(selected);
    }
}

void ControlRoot::prune()
{
    decltype(controls) new_controls;
    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        if (!itr.value().isNull())
            new_controls[itr.key()] = itr.value();
    controls = new_controls;
}
