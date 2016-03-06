#pragma once

#include <QObject>
#include <QHash>
#include <QPointer>

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
    InspectorFrame* getInspector() const { return inspector.data(); }

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
     *  Clears the 'Touched' flag for every Control
     */
    void clearControlTouched();

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

    /*
     *  Return a pointer to the target node
     */
    const Node* getNode() const { return node; }
    Node* getMutableNode() const { return node; }

protected:
    Node* const node;

    QHash<Datum*, DatumProxy*> datums;

    ScriptProxy* script;
    GraphProxy* subgraph;

    QMap<long, Control*> controls;

    /*  Strong pointer to inspector frame, to delete it when the node proxy
     *  is destroyed.  We use QPointer here instead of QScopedPointer because
     *  the pointer should be cleared if a window is closed (taking the
     *  InspectorFrames with it)    */
    QPointer<InspectorFrame> inspector;

    /*  Weak pointer to inspector buttons, as they are owned by the frame  */
    InspectorShowHiddenButton* show_hidden;
};
