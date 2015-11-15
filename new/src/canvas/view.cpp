#include "canvas/view.h"

CanvasView::CanvasView(Graph* g, QWidget* parent)
    : QGraphicsView(parent), g(g)
{
    // Nothing to do here
}
