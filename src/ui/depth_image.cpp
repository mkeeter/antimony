#include <Python.h>

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

#include "ui/depth_image.h"
#include "ui/canvas.h"

DepthImageItem::DepthImageItem(QImage depth, QImage shaded, Canvas* canvas)
    : QGraphicsItem(), depth(depth), shaded(shaded), canvas(canvas)
{
    // Nothing to do here
}

QRectF DepthImageItem::boundingRect() const
{
    return QRectF(0, 0, depth.width(), depth.height());
}

void DepthImageItem::paint(QPainter *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget *widget)
{
    QPoint a = canvas->mapFromScene(mapToParent(0, 0).toPoint());

    QImage blended(depth.width(), depth.height(), QImage::Format_RGBA8888);
    blended.fill(0);

    QImage* buffer = canvas->getDepth();
    for (int j=0; j < depth.height(); ++j)
    {
        if (a.y() + j < 0 || a.y() + j >= buffer->height())
        {
            continue;
        }
        for (int i=0; i < depth.width(); ++i)
        {
            if (a.x() + i < 0 || a.x() + i >= buffer->width())
            {
                continue;
            }

            if (depth.pixel(i, j) > buffer->pixel(i, j))
            {
                uint32_t pix = shaded.pixel(i, j);
                blended.setPixel(i, j, pix | pix << 8 | pix << 16);
                buffer->setPixel(a.x() + i, a.y() + j, depth.pixel(i, j));
            }
        }
    }
    buffer->save("out.png");
    painter->drawImage(0, 0, blended);
}
