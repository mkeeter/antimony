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
     *  Returns the datum proxy for the given datum, non-recursively
     *
     *  Checks first for a node proxy, then a subdatum proxy
     *  (returning NULL if neither exists)
     */
    BaseDatumProxy* getDatumProxy(const Datum* d) const;

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
    CanvasScene* canvas_scene;
    QList<QMainWindow*> windows;
    AppHooks* hooks;

    QHash<Node*, NodeProxy*> nodes;
    QHash<Datum*, SubdatumProxy*> datums;
};
