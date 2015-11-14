#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>

#include "graph/watchers.h"

class Node;
class GraphProxy;
class ScriptProxy;
class DatumProxy;

class NodeProxy : public QObject, public NodeWatcher
{
Q_OBJECT

public:
    NodeProxy(Node* n, GraphProxy* parent);
    void trigger(const NodeState& state) override;

protected:
    QHash<Datum*, QSharedPointer<DatumProxy>> datums;

    QSharedPointer<ScriptProxy> script;
    QSharedPointer<GraphProxy> subgraph;
};
