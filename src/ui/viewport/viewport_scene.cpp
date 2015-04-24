#include <Python.h>

#include "ui/viewport/viewport_scene.h"
#include "ui/viewport/viewport.h"
#include "render/render_worker.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "control/control.h"
#include "control/proxy.h"

ViewportScene::ViewportScene(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void ViewportScene::registerControl(Node* n, long index, Control* c)
{
    connect(c, &Control::glowChanged,
            this, &ViewportScene::onGlowChange);
    connect(c, &Control::glowChanged,
            this, &ViewportScene::glowChanged);

    controls[n][index] = c;

    for (auto v : viewports)
        makeProxyFor(c, v);
}

Control* ViewportScene::getControl(Node* n, long index) const
{
    if (!controls.contains(n))
        return NULL;
    if (!controls[n].contains(index))
        return NULL;
    if (controls[n][index].isNull())
        return NULL;
    return controls[n][index];
}

void ViewportScene::makeRenderWorkersFor(Node* n)
{
    for (auto v : viewports)
        makeRenderWorkersFor(n, v);

    // Behold, the wonders of C++11 and Qt5:
    connect(n, &Node::datumsChanged,
            [=]{ this->onDatumsChanged(n); });
}

Viewport* ViewportScene::newViewport()
{
    auto s = new QGraphicsScene;
    auto v = new Viewport(s);
    connect(v, &QObject::destroyed, s, &QObject::deleteLater);
    connect(s, &QObject::destroyed, this, &ViewportScene::prune);
    viewports << v;

    prune();

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
    {
        for (auto i = itr.value().begin(); i != itr.value().end(); ++i)
            if (!i.value().isNull())
                makeProxyFor(i.value(), v);
        makeRenderWorkersFor(itr.key(), v);
    }

    return v;
}

void ViewportScene::makeProxyFor(Control* c, Viewport* v)
{
    if (!c)
        return;

    connect(new ControlProxy(c, v), &ControlProxy::destroyed,
            this, &ViewportScene::prune);
}

void ViewportScene::makeRenderWorkerFor(Datum* d, Viewport* v)
{
    connect(new RenderWorker(d, v), &RenderWorker::destroyed,
            this, &ViewportScene::prune);
    workers << d;
}

void ViewportScene::makeRenderWorkersFor(Node* n, Viewport* v)
{
    // Add a dummy (NULL) control so that this node is stored and render
    // workers are created for it when a new viewport is made.
    controls[n][-1] = NULL;

    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d))
            makeRenderWorkerFor(d, v);
}

void ViewportScene::prune()
{
    decltype(controls) new_controls;

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        if (itr.key())
            new_controls[itr.key()] = itr.value();
    controls = new_controls;

    decltype(viewports) new_viewports;
    for (auto itr : viewports)
        if (itr)
            new_viewports << itr;
    viewports = new_viewports;

    decltype(workers) new_workers;
    for (auto itr : workers)
        if (itr)
            new_workers << itr;
    new_workers = workers;
}

void ViewportScene::onDatumsChanged(Node* n)
{
    prune();

    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d) && !workers.contains(d))
            for (auto v : viewports)
                makeRenderWorkerFor(d, v);
}


void ViewportScene::onGlowChange(Node* n, bool g)
{
    if (controls.contains(n))
        for (auto itr = controls[n].begin(); itr != controls[n].end(); ++itr)
            if (!itr.value().isNull())
                itr.value()->setGlow(g);
}

////////////////////////////////////////////////////////////////////////////////

uint qHash(const QPointer<Datum>& d) {
    return qHash(d.operator->());
}

uint qHash(const QPointer<Viewport>& v) {
    return qHash(v.operator->());
}

