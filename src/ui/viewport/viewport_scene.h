#ifndef VIEWPORT_SCENE_H
#define VIEWPORT_SCENE_H

#include <QPointer>
#include <QGraphicsScene>
#include <QMap>
#include <QList>

class Control;
class ControlProxy;
class Node;
class Datum;
class Viewport;
class RenderWorker;

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
     *  Creates Controls and DepthImageItems for this node.
     */
    void makeUIfor(Node* n);

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
    void makeProxyFor(Control* c, Viewport* v, ControlProxy* parent=NULL);

    /*
     *  Creates one or more RenderWorkers to render Datums in the
     *  given node that produces Shapes.
     */
    void makeRenderWorkersFor(Node* n, Viewport* v);

    /*
     *  Make a RenderWorker for the specified Datum in the given Viewport.
     */
    void makeRenderWorkerFor(Datum* d, Viewport* v);

    /*
     *  Makes a new control for the given node
     *  (hardcoded against the node's type code)
     */
    Control* makeControlFor(Node* n) const;

    QMap<QPointer<Viewport>, QGraphicsScene*> scenes;
    QMap<QPointer<Node>, Control*> controls;
    QMap<QPointer<Datum>, QList<QPointer<RenderWorker>>> workers;
};

#endif // VIEWPORT_SCENE_H
