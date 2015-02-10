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

void ViewportScene::registerControl(Control* c)
{
    connect(c, &Control::glowChanged,
            this, &ViewportScene::onGlowChange);
    connect(c, &Control::glowChanged,
            this, &ViewportScene::glowChanged);

    for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
        makeProxyFor(c, itr.key());
}

void ViewportScene::makeRenderWorkersFor(Node* n)
{
    for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
        makeRenderWorkersFor(n, itr.key());

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
    scenes[v] = s;

    prune();

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        for (auto i = itr.value().begin(); i != itr.value().end(); ++i)
        {
            makeProxyFor(i.value(), v);
            makeRenderWorkersFor(itr.key(), v);
        }

    return v;
}

void ViewportScene::makeProxyFor(Control* c, Viewport* v)
{
    if (!c)
        return;

    auto p = new ControlProxy(c, v);
    scenes[v]->addItem(p);

    connect(v, &Viewport::viewChanged,
            p, &ControlProxy::redraw);
    connect(c, &ControlProxy::destroyed,
            this, &ViewportScene::prune);
}

void ViewportScene::makeRenderWorkerFor(Datum* d, Viewport* v)
{
    auto w = new RenderWorker(d, v);
    workers[d] << w;
    connect(w, &RenderWorker::destroyed,
            this, &ViewportScene::prune);
}

void ViewportScene::makeRenderWorkersFor(Node* n, Viewport* v)
{
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

    QMap<QPointer<Viewport>, QGraphicsScene*> new_scenes;
    for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
        if (itr.key())
            new_scenes[itr.key()] = itr.value();
    scenes = new_scenes;

    decltype(workers) new_workers;
    for (auto itr = workers.begin(); itr != workers.end(); ++itr)
        if (itr.key())
            for (auto w : workers[itr.key()])
                if (w)
                    new_workers[itr.key()] << w;
    new_workers = workers;
}

void ViewportScene::onDatumsChanged(Node* n)
{
    prune();

    for (auto d : n->findChildren<Datum*>())
        if (RenderWorker::accepts(d) && !workers.contains(d))
            for (auto v = scenes.begin(); v != scenes.end(); ++v)
                makeRenderWorkerFor(d, v.key());
}


void ViewportScene::onGlowChange(Node* n, bool g)
{
    Q_ASSERT(controls.contains(n));
    for (auto itr = controls[n].begin(); itr != controls[n].end(); ++itr)
        itr.value()->setGlow(g);
}
