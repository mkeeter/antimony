#include <Python.h>

#include "graph/proxy/base_datum.h"
#include "canvas/connection/connection.h"

#include "viewport/render/instance.h"
#include "viewport/view.h"
#include "viewport/scene.h"

#include "graph/datum.h"

#include "fab/fab.h"

BaseDatumProxy::BaseDatumProxy(
        Datum* d, QObject* parent, ViewportScene* scene, bool sub)
    : QObject(parent), datum(d), sub(sub),
      should_render(d->getType() == fab::ShapeType)
{
    scene->installDatum(this);
}

BaseDatumProxy::~BaseDatumProxy()
{
    // I don't understand why this is necessary, but doing the deletions
    // directly from the QHash causes crashes.
    QList<Connection*> cs;
    for (auto c : connections)
        cs.push_back(c);
    for (auto c : cs)
        delete c;
}

////////////////////////////////////////////////////////////////////////////////

void BaseDatumProxy::addViewport(ViewportView* view)
{
    if (should_render)
    {
        auto r = new RenderInstance(this, view, sub);
        connect(view, &QObject::destroyed,
                r, &RenderInstance::makeOrphan);
        connect(this, &BaseDatumProxy::datumChanged,
                r, &RenderInstance::datumChanged);
    }
}
