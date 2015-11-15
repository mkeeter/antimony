#include "canvas/view.h"
#include "canvas/scene.h"

CanvasView::CanvasView(CanvasScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
{
    // Nothing to do here
}
