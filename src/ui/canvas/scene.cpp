#include <Python.h>

#include "ui/canvas/scene.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/connection.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/port.h"

#include "graph/datum/datum.h"

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
{
    // Nothing to do here
}

Canvas* GraphScene::newCanvas()
{
    return new Canvas(this);
}

NodeInspector* GraphScene::makeUIfor(Node* n)
{
    auto i = new NodeInspector(n);
    addItem(i);
    return i;
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
    for (auto i : items())
    {
        NodeInspector* c = dynamic_cast<NodeInspector*>(i);

        if (c && c->getNode() == node)
            return c;
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
    if (raised_inspector)
        raised_inspector->setZValue(-2);
    i->setZValue(-1.9);
    raised_inspector = i;
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
