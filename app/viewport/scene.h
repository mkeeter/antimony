#pragma once

#include <QObject>
#include <QSet>

class Graph;
class ViewportView;

class Control;
class BaseDatumProxy;

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
     *  Returns this scene's graph object without taking ownership
     */
    Graph* getGraph() const { return g; }

signals:
    /*
     *  Requests that every viewport calls installViewport on the given control
     */
    void installControl(Control* c);

    /*
     *  Requests that every viewport calls addViewport on the given proxy
     */
    void installDatum(BaseDatumProxy* d);

protected:
    Graph* const g;
};
