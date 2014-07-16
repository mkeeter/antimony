#ifndef DEPTH_IMAGE_H
#define DEPTH_IMAGE_H

#include <QGraphicsItem>
#include <QImage>

class Canvas;

class DepthImageItem : public QGraphicsItem
{
public:
    DepthImageItem(QImage depth, QImage shaded, Canvas* canvas);
    QRectF boundingRect() const;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage depth;
    QImage shaded;
    Canvas* canvas;
};

#endif // DEPTH_IMAGE_H
