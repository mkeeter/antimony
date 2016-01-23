#include <Python.h>

#include "viewport/scene.h"
#include "viewport/control/root.h"
#include "viewport/render/target.h"
#include "viewport/view.h"

ViewportScene::ViewportScene(Graph* g, QObject* parent)
    : QObject(parent), g(g)
{
    // Nothing to do here
}

void ViewportScene::addControl(ControlRoot* c)
{
    roots.push_back(c);
    connect(c, &QObject::destroyed, [=]{ this->roots.removeAll(c); });

    for (auto v : viewports)
    {
        c->makeInstancesFor(v);
    }
}

/*
void ViewportScene::addTarget(RenderTarget* t)
{
    targets.push_back(t);
    connect(t, &QObject::destroyed, [=]{ this->targets.removeAll(t); });

    for (auto v : viewports)
    {
        t->makeInstanceFor(v);
    }
}
*/

ViewportView* ViewportScene::getView(QWidget* parent)
{
    auto v = new ViewportView(parent);
    viewports.insert(v);

    for (auto r : roots)
    {
        r->makeInstancesFor(v);
    }

    /*
    for (auto t : targets)
    {
        t->makeInstanceFor(v);
    }
    */

    return v;
}
