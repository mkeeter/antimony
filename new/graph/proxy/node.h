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
class ExportWorker;

class InspectorFrame;

class NodeProxy : public QObject, public NodeWatcher
{
Q_OBJECT

public:
    NodeProxy(Node* n, GraphProxy* parent);
    ~NodeProxy();

    /*
     *  On node change, update datum proxies and name validity.
     */
    void trigger(const NodeState& state) override;

    /*
     *  Returns the node's inspector frame object.
     */
    InspectorFrame* getInspector() const { return inspector; }

    /*
     *  Records positions of all inspectors
     *
     *  Normally, this will be single-item map, but for GraphNodes
     *  we recurse down into subgraphs.
     */
    QMap<Node*, QPointF> inspectorPositions() const;

    /*
     *  Sets positions of all inspectors.
     *
     *  Normally, this checks against the single node, but we also
     *  recurse for GraphNodes.
     */
    void setInspectorPositions(const QMap<Node*, QPointF>& pos);

    /*
     *  Clears the inspector's 'Export' button
     */
    void clearExportWorker();

    /*
     *  Assigns the given export worker to the inspector's 'Export' button
     */
    void setExportWorker(ExportWorker* worker);

protected:
    Node* const node;

    QHash<Datum*, DatumProxy*> datums;

    ScriptProxy* script;
    GraphProxy* subgraph;

    InspectorFrame* inspector;
};
