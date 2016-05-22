#pragma once

#include <QObject>
#include <QHash>

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

public slots:
    /*
     *  Connected to the parent GraphProxy's subnameChanged signal
     *
     *  Ignores the incoming QString and emits subnameChanged with
     *  this node's fully qualified node name.
     */
    void onSubnameChanged(QString ignored="");

    /*
     *  Requests that inspectors and controls are zoomed to
     *  in their respective scenes
     */
    void onZoomTo();

signals:
    /*
     *  Signal connected to child GraphProxies that instructs them to
     *  change their window titles
     */
    void subnameChanged(QString subname);

    /*
     *  Used to set highlighting for inspector and controls
     */
    void setFocus(bool focus);

protected:
    Node* const node;

    QHash<Datum*, DatumProxy*> datums;

    ScriptProxy* script;
    GraphProxy* subgraph;

    QMap<long, Control*> controls;

    /*  Strong pointer to inspector frame.  We connect it to the proxy's
     *  destroyed signal, but it may also be deleted when the window is
     *  closed (since it's technically owned by the QGraphicsScene)  */
    InspectorFrame* inspector;

    /*  Weak pointer to inspector buttons, as they are owned by the frame  */
    InspectorShowHiddenButton* show_hidden;
};
