#ifndef DEPTH_IMAGE_H
#define DEPTH_IMAGE_H

#include <QGraphicsItem>
#include <QImage>

class Canvas;

class DepthImageItem : public QGraphicsItem
{
public:
    DepthImageItem(float zmin, float zmax, QImage depth, Canvas* canvas);
    QRectF boundingRect() const;

    float zmin;
    float zmax;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage depth;
    Canvas* canvas;
};

#endif // DEPTH_IMAGE_H
