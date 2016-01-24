#include <Python.h>

#include "viewport/scene.h"
#include "viewport/control/control.h"
#include "viewport/render/target.h"
#include "viewport/view.h"

ViewportScene::ViewportScene(Graph* g, QObject* parent)
    : QObject(parent), g(g)
{
    // Nothing to do here
}

ViewportView* ViewportScene::getView(QWidget* parent)
{
    auto v = new ViewportView(parent);
    viewports.push_back(v);
    return v;
}

void ViewportScene::makeInstancesFor(Control* c)
{
    for (auto v : viewports)
    {
        c->makeInstanceFor(v);
    }
}
