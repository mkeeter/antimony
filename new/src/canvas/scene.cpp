#include "canvas/scene.h"

CanvasScene::CanvasScene(Graph* g, QObject* parent)
    : QGraphicsScene(parent), g(g)
{
    // Nothing to do here
}
