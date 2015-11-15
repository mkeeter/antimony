#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>

#include "graph/watchers.h"

class Node;
class NodeProxy;
class SuperDatumProxy;

class CanvasScene;

class GraphProxy : public QObject, public GraphWatcher
{
Q_OBJECT

public:
    GraphProxy(Graph* g, QObject* parent);
    void trigger(const GraphState& state) override;

protected:
    QHash<Node*, QString> title_cache;

    CanvasScene* canvas_scene;

    QHash<Node*, QSharedPointer<NodeProxy>> nodes;
    QHash<Datum*, QSharedPointer<SuperDatumProxy>> datums;
};
