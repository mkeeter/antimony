#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QMainWindow>

#include "graph/watchers.h"
#include "canvas/info.h"

class Node;
class NodeProxy;
class BaseDatumProxy;
class SubdatumProxy;
class AppHooks;

class CanvasScene;
class CanvasWindow;

class GraphProxy : public QObject, public GraphWatcher
{
Q_OBJECT

public:
    GraphProxy(Graph* g, NodeProxy* parent=NULL);
    ~GraphProxy();

    /*
     *  Create NodeProxies and SubdatumProxies
     */
    void trigger(const GraphState& state) override;

    /*
     *  Returns the CanvasScene object
     */
    CanvasScene* canvasScene() const { return canvas_scene; }

    /*
     *  Constructs a new window of the given class, storing it in the
     *  windows list (and setting up automatic list pruning)
     */
    template <class W, class S> W* newWindow(S* scene);

    /*
     *  Opens a new Canvas window with this object's scene
     */
    CanvasWindow* newCanvasWindow();

    /*
     *  Dummy functions for viewport and quad view
     */
    void newViewWindow() {}
    void newQuadWindow() {}

    /*
     *  Returns a proxy for the given node, constructing one if none exists.
     *
     *  Just-in-time construction is necessary because script nodes call
     *  functions like sb.export.* before the graph has informed us of its
     *  new nodes.
     */
    NodeProxy* getNodeProxy(Node* g);

    /*
     *  Returns a datum for the given datum, constructing one if none exists.
     *
     *  Just-in-time construction is necessary because, when loading a file,
     *  connection targets may not already be created.
     *
     *  Checks first for whether the datum's parent belongs in this graph.
     *  If so, looks up the node proxy and gets the datum proxy from it.
     *
     *  Otherwise, the datum must be a subdatum belonging to this graph,
     *  so a subdatum proxy is constructed.
     *
     *  If neither of those cases is true, raises an assertion.
     */
    BaseDatumProxy* getDatumProxy(Datum* d);

    /*
     *  Records positions of all inspectors and subdatums
     *  (recursively descending into subgraphs)
     */
    CanvasInfo canvasInfo() const;

    /*
     *  Sets positions of all inspectors and subdatums
     *  (recursively descending into subgraphs)
     */
    void setPositions(const CanvasInfo& info);

protected:
    Graph* const graph;

    CanvasScene* canvas_scene;
    QList<QMainWindow*> windows;
    AppHooks* hooks;

    QHash<Node*, NodeProxy*> nodes;
    QHash<Datum*, SubdatumProxy*> datums;
};
