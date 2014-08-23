#ifndef RENDER_IMAGE_H
#define RENDER_IMAGE_H

#include <QObject>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QVector3D>

#include <cstdint>

#include "cpp/bounds.h"

struct Shape;
class Canvas;
class DepthImageItem;

class RenderImage : public QObject
{
    Q_OBJECT
public:
    explicit RenderImage(Bounds b, QVector3D pos, float scale);
    ~RenderImage();
    void render(Shape* shape);
    void applyGradient(bool direction);
    void addToCanvas(Canvas* canvas);

protected:
    /** Bounds assigned to output DepthImageItem
     *  (these may disagree with bounds on shape in render,
     *   but are only used for the output image.) */
    Bounds bounds;

    /** Corner position of output DepthImageItem
     *  (in scene coordinates) */
    QVector3D pos;

    /** Scale in pixels per unit */
    float scale;

    QImage depth;
    QMap<Canvas*, DepthImageItem*> pixmaps;
};

#endif // RENDER_IMAGE_H
