#include "ui/canvas/scene.h"
#include "ui/canvas/inspector/inspector.h"

GraphScene::GraphScene(QObject* parent)
    : QGraphicsScene(parent)
{
    // Nothing to do here
}

void GraphScene::makeUIfor(Node* n)
{
    addItem(new NodeInspector(n));
}
