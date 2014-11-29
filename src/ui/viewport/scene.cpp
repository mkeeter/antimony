#include <Python.h>

#include "ui/viewport/scene.h"
#include "ui/viewport/viewport.h"

#include "graph/node/node.h"
#include "control/control.h"
#include "control/proxy.h"

ViewportScene::ViewportScene(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

Viewport* ViewportScene::newViewport()
{
    auto s = new QGraphicsScene;
    auto v = new Viewport(s);
    scenes[v] = s;

    prune();

    for (auto itr = nodes.begin(); itr != nodes.end(); ++itr)
    {
        makeProxyFor(itr.value(), v);
        makeRenderWorkersFor(itr.key(), v);
    }

    return v;
}

void ViewportScene::makeProxyFor(Control* c, Viewport* v)
{
    auto p = new ControlProxy(c, v);
    scenes[v]->addItem(p);
}

void ViewportScene::makeRenderWorkersFor(Node* n, Viewport* v)
{
#warning "Not yet implemented"
}

void ViewportScene::prune()
{
    QMap<QPointer<Node>, Control*> new_nodes;

    for (auto itr = nodes.begin(); itr != nodes.end(); ++itr)
        if (itr.key())
            new_nodes[itr.key()] = itr.value();

    nodes = new_nodes;

    QMap<QPointer<Viewport>, QGraphicsScene*> new_scenes;
    for (auto itr = scenes.begin(); itr != scenes.end(); ++itr)
        if (itr.key())
            new_scenes[itr.key()] = itr.value();

    scenes = new_scenes;
}
