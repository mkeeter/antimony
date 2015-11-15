#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>

#include "graph/watchers.h"

#include "canvas/inspector/frame.h"

class Node;
class GraphProxy;
class ScriptProxy;
class DatumProxy;

class InspectorFrame;

class NodeProxy : public QObject, public NodeWatcher
{
Q_OBJECT

public:
    NodeProxy(Node* n, GraphProxy* parent);
    ~NodeProxy();

    void trigger(const NodeState& state) override;

protected:
    QHash<Datum*, DatumProxy*> datums;

    ScriptProxy* script;
    GraphProxy* subgraph;

    InspectorFrame* inspector;
};
