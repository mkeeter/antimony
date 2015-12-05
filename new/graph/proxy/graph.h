#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QMainWindow>

#include "graph/watchers.h"

class Node;
class NodeProxy;
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
     *  Returns a proxy for the given node,
     *  constructing one if no such proxy exists.
     */
    NodeProxy* getNodeProxy(Node* g);

    /*
     *  Records positions of all inspectors (recursively)
     */
    QMap<Node*, QPointF> inspectorPositions() const;

    /*
     *  Sets positions of all inspectors (recursively)
     */
    void setInspectorPositions(const QMap<Node*, QPointF>& pos);

protected:
    CanvasScene* canvas_scene;
    QList<QMainWindow*> windows;
    AppHooks* hooks;

    QHash<Node*, NodeProxy*> nodes;
    QHash<Datum*, SubdatumProxy*> datums;
};
