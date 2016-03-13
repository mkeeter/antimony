#include "canvas/info.h"

void CanvasInfo::unite(const CanvasInfo& other)
{
    inspector.unite(other.inspector);
    subdatum.unite(other.subdatum);
}
