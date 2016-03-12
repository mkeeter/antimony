#include <Python.h>

#include "viewport/scene.h"
#include "viewport/control/control.h"
#include "viewport/view.h"

ViewportScene::ViewportScene(Graph* g, QObject* parent)
    : QObject(parent), g(g)
{
    // Nothing to do here
}

ViewportView* ViewportScene::getView(QWidget* parent)
{
    auto v = new ViewportView(parent);
    connect(this, &ViewportScene::makeInstancesFor,
            v, &ViewportView::installControl);
    return v;
}
