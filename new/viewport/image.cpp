#include "viewport/image.h"
#include "viewport/view.h"

DepthImage::DepthImage(ViewportView* view)
{
    view->installImage(this);
}

void DepthImage::draw(QPainter* painter)
{
    painter->drawRect(QRect(0, 10, 0, 10));
}
