#ifndef DEPTH_IMAGE_H
#define DEPTH_IMAGE_H

#include <QGraphicsObject>
#include <QImage>
#include <QVector3D>

class Canvas;

class DepthImageItem : public QGraphicsObject
{
    Q_OBJECT
public:
    DepthImageItem(QVector3D pos, QVector3D size, QImage depth, Canvas* canvas);
    QRectF boundingRect() const;

    /** Position of lower-left corner (in original scene units) */
    QVector3D pos;

    /** Scale (in rotated scene coordinates) */
    QVector3D size;

public slots:
    void reposition();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    QImage depth;
    Canvas* canvas;
};

#endif // DEPTH_IMAGE_H
