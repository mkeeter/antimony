#ifndef VIEWPORT_SCENE_H
#define VIEWPORT_SCENE_H

#include <QPointer>
#include <QSharedPointer>
#include <QGraphicsScene>
#include <QMap>
#include <QSet>
#include <QList>

#include "util/hash.h"

class Control;
class ControlRoot;
class ControlProxy;
class Datum;
class Node;
class Viewport;

class ViewportScene : public QObject
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
    ViewportScene(QObject* parent=0);

    /*
     *  Returns a new viewport
     *  (with appropriate Controls and DepthImageItems already made)
     */
    Viewport* newViewport();

    /*
     *  Creates DepthImageItems for this node.
     */
    void makeRenderWorkersFor(Node* n);

    /*
     *  Registers a Control object, making proxies.
     */
    void registerControl(Node* n, long index, Control* c);

    /*
     *  Looks up a particular control by node and index.
     */
    Control* getControl(Node* node, long index) const;

public slots:
    void onGlowChange(Node* n, bool g);

signals:
    /*
     *  Used to cross-link glow between viewport and canvas.
     */
    void glowChanged(Node* n, bool g);

protected slots:
    /*
     *  Removes dead Viewport and Node pointers from the list and map.
     */
    void prune();

    /*
     *  When a Node's datums change, update RenderWorkers.
     */
    void onDatumsChanged(Node* n);

protected:

    /*
     *  Creates a ControlProxy that represents the given Control
     *  in the given QGraphicsScene.
     */
    void makeProxyFor(Control* c, Viewport* v);

    /* Stores viewports for which we've made a QGraphicsScene */
    QSet<QPointer<Viewport>> viewports;

    /* Score a set of top-level control roots
     * (which manage highlighting and glowing)
     */
    QMap<QPointer<Node>, QSharedPointer<ControlRoot>> controls;

    /* Stores Datums for which we have created RenderWorkers */
    QSet<QPointer<Datum>> workers;
};

#endif // VIEWPORT_SCENE_H
