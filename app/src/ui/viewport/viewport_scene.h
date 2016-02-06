#ifndef VIEWPORT_SCENE_H
#define VIEWPORT_SCENE_H

#include <QPointer>
#include <QSharedPointer>
#include <QGraphicsScene>
#include <QMap>
#include <QSet>
#include <QList>

#include "graph/watchers.h"

class Control;
class ControlRoot;
class Viewport;

class Datum;
class Node;
class Graph;

class ViewportScene : public QObject, public GraphWatcher
{
    Q_OBJECT
public:
    /*
     *  A ViewportScene is similar in nature to the CanvasScene
     *
     *  It secretly contains multiple QGraphicsScenes
     *  (one for each Viewport), as they are a fundamentally
     *  2D abstration and each Viewport is a 2D projection of
     *  the same 3D scene.
     */
    ViewportScene(Graph* root, QObject* parent=0);

    /*
     *  Returns a new viewport
     *  (with appropriate Controls and DepthImageItems already made)
     */
    Viewport* newViewport();

    /*
     *  On graph change, delete controls that are now orphaned.
     */
    void trigger(const GraphState& state) override;

    /*
     *  Registers a Control object, making proxies.
     */
    void registerControl(Node* n, long index, Control* c);

    /*
     *  Looks up a particular control by node and index.
     */
    Control* getControl(Node* node, long index) const;

    /*
     *  If a render worker exists for the given node and datum,
     *  trigger a re-render check (used when connections are made
     *  and broken).
     */
    void checkRender(Node* n, Datum* d);

    /*
     *  When loading is set, we don't create ControlRoot objects for nodes
     *  (to avoid creating a glut of rendering threads when many shapes are
     *  in fact connected and don't need to be rendered)
     */
    bool loading;

public slots:
    void onGlowChange(Node* n, bool g);

signals:
    /*
     *  Used to cross-link glow between viewport and canvas.
     */
    void glowChanged(Node* n, bool g);

protected:

    /*
     *  Creates a ControlProxy that represents the given Control
     *  in the given QGraphicsScene.
     */
    void makeProxyFor(Control* c, Viewport* v);

    /* Stores viewports for which we've made a QGraphicsScene */
    QSet<Viewport*> viewports;

    /* Store a set of top-level control roots
     * (which manage UI hooks, render workers, highlighting, glowing)
     */
    QMap<Node*, QSharedPointer<ControlRoot>> controls;

    friend class ControlRoot;
};

#endif // VIEWPORT_SCENE_H
