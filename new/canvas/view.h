#pragma once

#include <QGraphicsView>

class Graph;
class CanvasScene;

class CanvasView : public QGraphicsView
{
public:
    CanvasView(CanvasScene* scene, QWidget* parent);
};
