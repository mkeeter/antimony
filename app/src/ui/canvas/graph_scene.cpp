#include <Python.h>

#include "ui/canvas/graph_scene.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/connection.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/port.h"

#include "app/app.h"
#include "app/undo/undo_move.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
{
    connect(this, &GraphScene::jumpTo,
            App::instance(), &App::jumpToInViewport);
}

Canvas* GraphScene::newCanvas()
{
    return new Canvas(this);
}

void GraphScene::onGlowChange(Node* n, bool g)
{
    getInspector(n)->setGlow(g);
}

void GraphScene::makeUIfor(Node* n)
{
    auto i = new NodeInspector(n);
    get_inspector_cache[n] = i;
    addItem(i);

    if (views().length() > 0)
    {
        Q_ASSERT(dynamic_cast<Canvas*>(views().back()));
        auto c = static_cast<Canvas*>(views().back());
        i->setPos(c->mapToScene(c->geometry().center()) -
                  QPointF(i->boundingRect().width()/2,
                          i->boundingRect().height()/2));
    }

    connect(i, &NodeInspector::glowChanged,
            this, &GraphScene::onGlowChange);
    connect(i, &NodeInspector::glowChanged,
            this, &GraphScene::glowChanged);

    for (auto d : n->findChildren<Datum*>())
        for (auto link : d->findChildren<Link*>())
            makeUIfor(link);
}

Connection* GraphScene::makeUIfor(Link* link)
{
    auto c = new Connection(link);
    addItem(c);
    c->makeSceneConnections();
    return c;
}

NodeInspector* GraphScene::getInspector(Node* node)
{
    if (get_inspector_cache.contains(node))
    {
        auto c = get_inspector_cache[node];
        if (!c.isNull() && c->getNode() == node)
            return c;
        else
            get_inspector_cache.remove(node);
    }

    for (auto i : items())
    {
        NodeInspector* c = dynamic_cast<NodeInspector*>(i);

        if (c && c->getNode() == node)
        {
            get_inspector_cache[node] = c;
            return c;
        }
    }
    return NULL;
}

template <class T>
T* GraphScene::getItemAt(QPointF pos)
{
    for (auto i : items(pos))
        if (auto p = dynamic_cast<T*>(i))
            return p;
    return NULL;
}

NodeInspector* GraphScene::getInspectorAt(QPointF pos)
{
    return getItemAt<NodeInspector>(pos);
}

InputPort* GraphScene::getInputPortAt(QPointF pos)
{
    return getItemAt<InputPort>(pos);
    for (auto i : items(pos))
        if (auto p = dynamic_cast<InputPort*>(i))
            return p;
    return NULL;
}

InputPort* GraphScene::getInputPortNear(QPointF pos, Link* link)
{
    float distance = INFINITY;
    InputPort* port = NULL;

    for (auto i : items())
    {
        InputPort* p = dynamic_cast<InputPort*>(i);
        if (p && (link == NULL || p->getDatum()->acceptsLink(link)))
        {
            QPointF delta = p->mapToScene(p->boundingRect().center()) - pos;
            float d = QPointF::dotProduct(delta, delta);
            if (d < distance)
            {
                distance = d;
                port = p;
            }
        }
    }

    return port;
}

void GraphScene::raiseInspector(NodeInspector* i)
{
    removeItem(i);
    addItem(i);
}

void GraphScene::raiseInspectorAt(QPointF pos)
{
    auto i = getInspectorAt(pos);
    if (i)
        raiseInspector(i);
}

QMap<Node*, QPointF> GraphScene::inspectorPositions() const
{
    QMap<Node*, QPointF> out;

    for (auto m : items())
        if (auto i = dynamic_cast<NodeInspector*>(m))
            out[i->getNode()] = i->pos();

    return out;
}

void GraphScene::setInspectorPositions(QMap<Node*, QPointF> p)
{
    for (auto m : items())
        if (auto i = dynamic_cast<NodeInspector*>(m))
            if (p.contains(i->getNode()))
                i->setPos(p[i->getNode()]);
}

void GraphScene::endDrag(QPointF delta)
{
    App::instance()->beginUndoMacro("'drag'");
    for (auto m : selectedItems())
        if (auto i = dynamic_cast<NodeInspector*>(m))
            App::instance()->pushStack(new UndoMoveCommand(
                        this, i->getNode(), i->pos() - delta, i->pos()));
    App::instance()->endUndoMacro();
}
