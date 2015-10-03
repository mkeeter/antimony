#include <Python.h>

#include "ui/viewport/viewport_scene.h"
#include "ui/viewport/viewport.h"
#include "render/render_worker.h"

#include "graph/node.h"
#include "graph/datum.h"
#include "graph/graph.h"

#include "control/control.h"
#include "control/control_root.h"
#include "control/proxy.h"

ViewportScene::ViewportScene(Graph* root, QObject* parent)
    : QObject(parent), loading(false)
{
    root->installWatcher(this);
}

void ViewportScene::registerControl(Node* n, long index, Control* c)
{
    connect(c, &Control::glowChanged,
            this, &ViewportScene::onGlowChange);
    connect(c, &Control::glowChanged,
            this, &ViewportScene::glowChanged);

    if (!controls.contains(n))
        controls[n].reset(new ControlRoot(n, this));
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

Viewport* ViewportScene::newViewport()
{
    auto s = new QGraphicsScene;
    auto v = new Viewport(s);
    connect(v, &QObject::destroyed, s, &QObject::deleteLater);
    connect(s, &QObject::destroyed, [=](){ this->viewports.remove(v); });
    viewports.insert(v);

    for (auto itr = controls.begin(); itr != controls.end(); ++itr)
        itr.value()->makeProxiesFor(v);

    return v;
}

void ViewportScene::trigger(const GraphState& state)
{
    for (auto n : state.nodes)
        // If this node is one that we don't have a ControlRoot for,
        // create one (except in the special case when loading is set,
        // which indicates that we're loading the scene)
        if (!loading && !controls.contains(n))
            controls[n].reset(new ControlRoot(n, this));

    auto itr = controls.begin();
    while (itr != controls.end())
        if (state.nodes.count(itr.key()))
            itr++;
        else
            itr = controls.erase(itr);
}

void ViewportScene::onGlowChange(Node* n, bool g)
{
    if (controls.contains(n))
        controls[n]->setGlow(g);
}

void ViewportScene::checkRender(Node* n, Datum* d)
{
    if (controls.contains(n))
        controls[n]->checkRender(d);
}
