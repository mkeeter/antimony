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
    QImage& zbuffer = *canvas->getDepth();

    // We need to transform the depth image into global z space
    QImage depth_(zbuffer.width(), zbuffer.height(), zbuffer.format());
    depth_.fill(0x0); // dummy scale for now
    {
        QPainter p(&depth_);

        // Apply scale to depth-map
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawImage(a.x(), a.y(), depth);

        // Apply scale
        QImage scale(depth.width(), depth.height(), depth.format());
        scale.fill(0xffffff);
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawImage(a.x(), a.y(), scale);

        // Apply offset
        QImage offset(depth.width(), depth.height(), depth.format());
        offset.fill(0x0); // dummy offset for now
        p.setCompositionMode(QPainter::CompositionMode_Plus);
        p.drawImage(a.x(), a.y(), offset);

        // Multiply by a basic mask so that we have zeros outside of original
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawImage(a.x(), a.y(),
                    depth.createMaskFromColor(0xff000000, Qt::MaskOutColor));
    }

    QImage mask = zbuffer;
    // Update buffer with lightened height-map.
    {
        QPainter p(&zbuffer);
        p.setCompositionMode(QPainter::CompositionMode_Lighten);
        p.drawImage(0, 0, depth_);
    }

    zbuffer.save("zbuffer.png");

    // Subtract old buffer from new buffer.
    {
        QPainter p(&mask);
        p.setCompositionMode(QPainter::CompositionMode_Difference);
        p.drawImage(0, 0, zbuffer);
    }

    mask = mask.createMaskFromColor(0, Qt::MaskOutColor);

    QImage masked = shaded;
    {
        QPainter p(&masked);
        p.setCompositionMode(QPainter::CompositionMode_Multiply);
        p.drawImage(0, 0, mask);
    }

    painter->drawImage(0, 0, masked);
}
