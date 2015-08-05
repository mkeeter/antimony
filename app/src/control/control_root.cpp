#include <Python.h>

#include "control/control_root.h"
#include "control/control.h"
#include "control/proxy.h"

#include "render/render_worker.h"
#include "render/render_proxy.h"
#include "ui/viewport/viewport.h"
#include "ui/viewport/viewport_scene.h"

#include "graph/node.h"

ControlRoot::ControlRoot(Node* n, ViewportScene* vs)
    : vscene(vs), selected(false)
{
    n->installWatcher(this);
    trigger(n->getState());
}

void ControlRoot::registerControl(long index, Control* c)
{
    controls[index].reset(c);
    connect(c, &Control::proxySelectionChanged,
            this, &ControlRoot::changeProxySelection);
    connect(c, &Control::proxySelectionChanged,
            [=](bool s){ this->selected = s; });
    connect(this, &ControlRoot::changeProxySelection,
            c, &Control::changeProxySelection);
}

void ControlRoot::checkRender(Datum* d)
{
    if (workers.contains(d))
        emit(workers[d]->changed());
}

void ControlRoot::trigger(const NodeState& state)
{
    // Make new render workers for any datums that have been added
    for (auto d : state.datums)
        if (RenderWorker::accepts(d) && !workers.contains(d))
        {
            workers[d].reset(new RenderWorker(d));
            for (auto v : vscene->viewports)
                new RenderProxy(workers[d].data(), v);
        }

    {   // Delete any worker that is no longer in the datums list
        auto itr = workers.begin();
        while (itr != workers.end())
            if (std::find(state.datums.begin(), state.datums.end(), itr.key())
                    == state.datums.end())
                itr = workers.erase(itr);
            else
                itr++;
    }

    {   // Delete any controls that weren't touched in the last script exec
        auto itr = controls.begin();
        while (itr != controls.end())
            if (itr.value()->checkTouched())
                itr++;
            else
                itr = controls.erase(itr);
    }
}

Control* ControlRoot::get(long index) const
{
    if (!controls.contains(index) || controls[index].isNull())
        return NULL;
    return controls[index].data();
}

void ControlRoot::setGlow(bool g)
{
    for (auto itr=controls.begin(); itr != controls.end(); ++itr)
        if (!itr.value().isNull())
            itr.value()->setGlow(g);
}

void ControlRoot::makeProxiesFor(Viewport* v)
{
    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
    {
        auto p = new ControlProxy(itr.value().data(), v);
        if (selected)
            p->selectProxy(selected);
    }

    for (auto itr=workers.begin(); itr != workers.end(); ++itr)
        new RenderProxy(itr.value().data(), v);
}
