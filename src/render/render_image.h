#ifndef RENDER_IMAGE_H
#define RENDER_IMAGE_H

#include <QObject>
#include <QImage>
#include <QGraphicsPixmapItem>

#include <cstdint>

#include "cpp/bounds.h"

struct Shape;
class Canvas;
class DepthImageItem;

class RenderImage : public QObject
{
    Q_OBJECT
public:
    explicit RenderImage(Shape* shape, QObject* parent=0);
    ~RenderImage();
    void render(Shape* shape);
    void applyGradient(bool direction);
    void addToCanvas(Canvas* canvas);
    void setZ(float zmin, float zmax);

protected:
    Bounds bounds;
    QImage depth;
    QMap<Canvas*, DepthImageItem*> pixmaps;
};

#endif // RENDER_IMAGE_H
