#pragma once

#include <QObject>
#include <QSet>

class Graph;
class ViewportView;

class ControlRoot;
class RenderTarget;

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
    ViewportScene(Graph* root, QObject* parent=0);

    /*
     *  Returns a new ViewportView looking at this scene
     */
    ViewportView* getView(QWidget* parent=NULL);

    /*
     *  Adds a new control root
     */
    void addControl(ControlRoot* c);

    /*
     *  Adds a new render target without taking ownership
     */
    /*
    void addTarget(RenderTarget* t);
    */

    /*
     *  Returns this scene's graph object without taking ownership
     */
    Graph* getGraph() const { return g; }

protected:
    Graph* const g;

    /* Stores viewports for which we've made a QGraphicsScene */
    QSet<ViewportView*> viewports;

    QList<ControlRoot*> roots;
    //QList<RenderTarget*> targets;
};
