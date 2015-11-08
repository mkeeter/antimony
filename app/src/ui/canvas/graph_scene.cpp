#include <Python.h>

#include "ui/canvas/graph_scene.h"
#include "ui/canvas/canvas.h"
#include "ui/canvas/connection.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/canvas/port.h"
#include "ui/util/scene.h"

#include "app/app.h"
#include "app/undo/undo_move.h"

#include "graph/datum.h"
#include "graph/graph.h"

GraphScene::GraphScene(Graph* graph, QObject* parent)
    : QGraphicsScene(parent)
{
    graph->installWatcher(this);
    connect(this, &GraphScene::jumpTo,
            App::instance(), &App::jumpToInViewport);
}

GraphScene::~GraphScene()
{
    for (auto i : inspectors)
        i->deleteLater();
}

Canvas* GraphScene::newCanvas()
{
    return new Canvas(this);
}

void GraphScene::trigger(const GraphState& state)
{
    QSet<Node*> nodes;
    for (auto n : state.nodes)
        nodes.insert(n);

    auto itr = inspectors.begin();
    while (itr != inspectors.end())
    {
        if (!nodes.contains(itr.key()))
        {
            itr.value()->deleteLater();
            itr = inspectors.erase(itr);
        }
        else
        {
            itr++;
        }
    }

    for (auto n : nodes)
        if (!inspectors.contains(n))
            makeUIfor(n);
}

void GraphScene::onGlowChange(Node* n, bool g)
{
    getInspector(n)->setGlow(g);
}

void GraphScene::makeUIfor(Node* n)
{
    auto i = new NodeInspector(n);
    inspectors[n] = i;
    addItem(i);

    if (views().length() > 0)
    {
        Q_ASSERT(dynamic_cast<Canvas*>(views().back()));
        auto c = static_cast<Canvas*>(views().back());
        i->setPos(c->mapToScene(c->geometry().center()) -
                  QPointF(i->boundingRect().width()/2,
                          i->boundingRect().height()/2));
    }

    // If we've cached a title (e.g. because a Node called title
    // before its inspector was created), assign it here.
    if (title_cache.contains(n))
    {
        i->setTitle(title_cache[n]);
        title_cache.remove(n);
    }
    if (export_cache.contains(n))
    {
        i->setExportWorker(export_cache[n]);
        export_cache.remove(n);
    }

    connect(i, &NodeInspector::glowChanged,
            this, &GraphScene::onGlowChange);
    connect(i, &NodeInspector::glowChanged,
            this, &GraphScene::glowChanged);

    /*
    for (auto d : n->findChildren<Datum*>())
        for (auto link : d->findChildren<Link*>())
            makeUIfor(link);
            */
}

Connection* GraphScene::makeLinkFrom(Datum* d)
{
    auto c = new Connection(inspectors[d->parentNode()]->outputPort(d));
    addItem(c);
    return c;
}

void GraphScene::makeLink(const Datum* source, InputPort* target)
{
    Q_ASSERT(inspectors.contains(source->parentNode()));
    inspectors[source->parentNode()]->makeLink(source, target);
}

NodeInspector* GraphScene::getInspector(Node* node) const
{
    Q_ASSERT(inspectors.contains(node));
    return inspectors[node];
}

template <class T>
T* GraphScene::getItemAt(QPointF pos) const
{
    for (auto i : items(pos))
        if (auto p = dynamic_cast<T*>(i))
            return p;
    return NULL;
}

void GraphScene::setTitle(Node* node, QString title)
{
    if (inspectors.contains(node))
        inspectors[node]->setTitle(title);
    else
        title_cache[node] = title;
}

void GraphScene::clearExportWorker(Node* node)
{
    if (inspectors.contains(node))
        inspectors[node]->clearExportWorker();
    else if (export_cache.contains(node))
        export_cache.remove(node);
}

void GraphScene::setExportWorker(Node* node, ExportWorker* worker)
{
    if (inspectors.contains(node))
        inspectors[node]->setExportWorker(worker);
    else
        export_cache[node] = worker;
}

NodeInspector* GraphScene::getInspectorAt(QPointF pos) const
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

InputPort* GraphScene::getInputPortNear(QPointF pos, Datum* d)
{
    float distance = INFINITY;
    InputPort* port = NULL;

    for (auto i : items())
    {
        InputPort* p = dynamic_cast<InputPort*>(i);
        if (p && p->isVisible() && (d == NULL ||
                                    p->getDatum()->acceptsLink(d)))
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
