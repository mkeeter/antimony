#include <Python.h>

#include "ui/viewport/viewport_scene.h"
#include "ui/viewport/viewport.h"
#include "render/render_worker.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "control/control.h"
#include "control/control_root.h"
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

    if (!controls.contains(n))
        controls[n] = QSharedPointer<ControlRoot>(new ControlRoot(n));
    controls[n]->registerControl(index, c);

    for (auto v : viewports)
        new ControlProxy(c, v);
}

Control* ViewportScene::getControl(Node* n, long index) const
{
    if (!controls.contains(n))
        return NULL;
    return controls[n]->get(index);
}

void ViewportScene::makeRenderWorkersFor(Node* n)
{
    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d))
        {
            workers.insert(d);
            for (auto v : viewports)
                connect(new RenderWorker(d, v), &RenderWorker::destroyed,
                        this, &ViewportScene::prune);
        }

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
        itr.value()->makeProxiesFor(v);

    // Make new render workers for the added viewport
    for (auto w : workers)
        connect(new RenderWorker(w, v), &RenderWorker::destroyed,
                this, &ViewportScene::prune);

    return v;
}

void ViewportScene::prune()
{
    decltype(controls) new_controls;

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        if (itr.key())
        {
            new_controls[itr.key()] = itr.value();
            new_controls[itr.key()]->prune();
        }
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
    workers = new_workers;
}

void ViewportScene::onDatumsChanged(Node* n)
{
    prune();

    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d) && !workers.contains(d))
        {
            workers.insert(d);
            for (auto v : viewports)
                connect(new RenderWorker(d, v), &RenderWorker::destroyed,
                        this, &ViewportScene::prune);
        }
}


void ViewportScene::onGlowChange(Node* n, bool g)
{
    if (controls.contains(n))
        controls[n]->setGlow(g);
}
