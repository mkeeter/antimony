#pragma once

#include <QGraphicsView>

class Graph;

class CanvasView : public QGraphicsView
{
public:
    CanvasView(Graph* g, QWidget* parent);
protected:
    Graph* g;
};
