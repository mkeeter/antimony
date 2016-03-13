#include <Python.h>

#include "viewport/scene.h"
#include "viewport/control/control.h"
#include "viewport/view.h"
#include "graph/proxy/datum.h"

ViewportScene::ViewportScene(Graph* g, QObject* parent)
    : QObject(parent), g(g)
{
    // Nothing to do here
}

ViewportView* ViewportScene::getView(QWidget* parent)
{
    auto v = new ViewportView(parent, this);
    connect(this, &ViewportScene::installControl,
            v, &ViewportView::installControl);
    connect(this, &ViewportScene::installDatum,
            v, &ViewportView::installDatum);
    return v;
}
