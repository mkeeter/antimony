#pragma once

#include <QGraphicsScene>

class Graph;
class CanvasView;

class CanvasScene : public QGraphicsScene
{
public:
    CanvasScene(Graph* g, QObject* parent);

    /*
     *  Returns a new CanvasView object looking at this scene
     */
    CanvasView* getView(QWidget* parent=NULL);

    /*
     *  Returns this scene's graph object
     */
    Graph* getGraph() const { return g; }

    /*
     *  Creates an undo command that undoes a drag operation
     */
    void endDrag(QPointF delta);

protected:
    Graph* const g;
};
