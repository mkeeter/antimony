#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>

#include "graph/watchers.h"

#include "canvas/inspector/frame.h"
#include "canvas/info.h"

class Node;
class GraphProxy;
class ScriptProxy;
class DatumProxy;
class ExportWorker;
class Control;
class ViewportView;

class InspectorFrame;
class InspectorShowHiddenButton;

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
     *  Records positions of all inspectors and subdatums
     *
     *  Normally, this will be a single item in the inspector member
     *  variablej, but for GraphNodes we recurse down into subgraphs.
     */
    CanvasInfo canvasInfo() const;

    /*
     *  Sets positions of all inspectors and subdatums
     *
     *  Normally, this checks against the single node, but we also
     *  recurse for GraphNodes.
     */
    void setPositions(const CanvasInfo& info, bool select=false);

    /*
     *  Clears the inspector's 'Export' button
     */
    void clearExportWorker();

    /*
     *  Assigns the given export worker to the inspector's 'Export' button
     */
    void setExportWorker(ExportWorker* worker);

    /*
     *  Look up the proxy for the given datum, constructing one if necessary
     */
    DatumProxy* getDatumProxy(Datum* d);

    /*
     *  Make Control and Render instances for a new viewport view
     */
    void makeInstancesFor(ViewportView* v);

    /*
     *  Look up a control by line number in the control root
     *  (returning nullptr if none is found)
     */
    Control* getControl(long lineno);

    /*
     *  Register a control, constructing appropriate instances
     */
    void registerControl(long lineno, Control* c);

protected:
    Node* const node;

    QHash<Datum*, DatumProxy*> datums;

    ScriptProxy* script;
    GraphProxy* subgraph;

    QMap<long, Control*> controls;

    InspectorFrame* inspector;
    InspectorShowHiddenButton* show_hidden;
};
