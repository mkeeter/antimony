#ifndef RENDER_IMAGE_H
#define RENDER_IMAGE_H

#include <QObject>
#include <QImage>
#include <QGraphicsPixmapItem>

#include "cpp/bounds.h"

struct Shape;
class Canvas;

class RenderImage : public QObject
{
    Q_OBJECT
public:
    explicit RenderImage(Shape* shape, QObject* parent=0);
    ~RenderImage();
    void addToCanvas(Canvas* canvas);

protected:
    Bounds bounds;
    QImage image;
    QMap<Canvas*, QGraphicsPixmapItem*> pixmaps;
};

#endif // RENDER_IMAGE_H
