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

class ViewportScene;
class ViewportWindow;
class QuadWindow;
class Control;

class GraphProxy : public QObject, public GraphWatcher
{
Q_OBJECT

public:
    GraphProxy(Graph* g, NodeProxy* parent=NULL);

    /*
     *  Create NodeProxies and SubdatumProxies
     */
    void trigger(const GraphState& state) override;

    /*
     *  Returns the CanvasScene object
     */
    CanvasScene* canvasScene() const { return canvas_scene; }

    /*
     *  Returns the ViewportScene object
     */
    ViewportScene* viewportScene() const { return viewport_scene; }

    /*
     *  Constructs a new window of the given class, connecting it so that it
     *  will be deleted when this graph proxy is destroyed.
     */
    template <class W, class S> W* newWindow(S* scene);

    /*
     *  Opens a new Canvas window with this object's scene
     */
    CanvasWindow* newCanvasWindow();

    /*
     *  Opens a new Viewport window with this object's scene
     */
    ViewportWindow* newViewportWindow();

    /*
     *  Opens a new Viewport window with this object's scene
     */
    QuadWindow* newQuadWindow();

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
    void setPositions(const CanvasInfo& info, bool select=false);

    /*
     *  Make instances for a new control
     */
    void makeInstancesFor(Control* c);

signals:
    /*
     *  Signal connected to windows that renames them
     */
    void subnameChanged(QString n);

    /*
     *  Zooms to the given node in all viewports and canvases
     */
    void zoomTo(Node* n);

protected:
    Graph* const graph;

    CanvasScene* canvas_scene;
    ViewportScene* viewport_scene;
    AppHooks* hooks;

    QHash<Node*, NodeProxy*> nodes;
    QHash<Datum*, SubdatumProxy*> datums;
};
