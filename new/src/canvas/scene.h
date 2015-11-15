#pragma once

#include <QGraphicsScene>

class Graph;

class CanvasScene : public QGraphicsScene
{
public:
    CanvasScene(Graph* g, QObject* parent);
protected:
    Graph* g;
};
