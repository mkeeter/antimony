#include "canvas/scene.h"
#include "canvas/view.h"

CanvasScene::CanvasScene(Graph* g, QObject* parent)
    : QGraphicsScene(parent), g(g)
{
    // Nothing to do here
}

CanvasView* CanvasScene::getView(QWidget* parent)
{
    return new CanvasView(this, parent);
}
