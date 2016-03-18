#pragma once

#include <QGraphicsScene>

class Graph;
class CanvasView;
class InputPort;
class Datum;

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

    /*
     *  Returns the top item of the given class at the given position
     */
    template <class T> T* itemAt(QPointF pos) const;

    /*
     *  Returns the input port nearest to the given point
     *  that accepts the given datum as an incoming link.
     */
    InputPort* inputPortNear(QPointF pos, Datum* source=NULL) const;

    /*
     *  Returns the top input port at the given position
     */
    InputPort* inputPortAt(QPointF pos) const;

protected:
    Graph* const g;
};
