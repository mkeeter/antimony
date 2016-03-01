#include "viewport/render/instance.h"
#include "graph/proxy/datum.h"

RenderInstance::RenderInstance(DatumProxy* parent, ViewportView* view)
    : QObject(parent), image(view)
{
    // Nothing to do here
}
