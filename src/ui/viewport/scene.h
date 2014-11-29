#ifndef VIEWPORT_SCENE_H
#define VIEWPORT_SCENE_H

#include <QPointer>
#include <QGraphicsScene>
#include <QMap>
#include <QList>

class Control;
class Node;
class Viewport;

class ViewportScene : public QObject
{
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
     *  Creates one or more Controls and DepthImageItems for this node.
     */
    Control* makeUIfor(Node* n);

protected:
    /*
     *  Creates a ControlProxy that represents the given Control
     *  in the given QGraphicsScene.
     */
    void makeProxyFor(Control* c, Viewport* v);

    /*
     *  Creates one or more RenderWorkers to render Datums in the
     *  given node that produces Shapes.
     */
    void makeRenderWorkersFor(Node* n, Viewport* v);

    /*
     *  Removes dead Viewport and Node pointers from the list and map.
     */
    void prune();

    QMap<QPointer<Viewport>, QGraphicsScene*> scenes;
    QMap<QPointer<Node>, Control*> nodes;
};

#endif // VIEWPORT_SCENE_H
