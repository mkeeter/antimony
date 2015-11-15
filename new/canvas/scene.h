#pragma once

#include <QGraphicsScene>

class Graph;
class CanvasView;

class CanvasScene : public QGraphicsScene
{
public:
    CanvasScene(Graph* g, QObject* parent);
    CanvasView* getView(QWidget* parent=NULL);

    Graph* getGraph() const { return g; }
protected:
    Graph* g;
};
