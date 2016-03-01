#include "viewport/image.h"
#include "viewport/view.h"

DepthImage::DepthImage(ViewportView* view)
{
    view->installImage(this);
}
