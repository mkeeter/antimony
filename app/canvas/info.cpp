#include "canvas/info.h"

void CanvasInfo::unite(const CanvasInfo& other)
{
    inspector.insert(other.inspector);
    subdatum.insert(other.subdatum);
}
