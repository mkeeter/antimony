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

    /** Sets the normals in the shaded image.
     *  (used for 2D image shading).
     */
    void setNormals(float xy, float z);

public slots:
    void halt();

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
    QImage shaded;
    QMap<Canvas*, DepthImageItem*> pixmaps;

    int halt_flag;
};

#endif // RENDER_IMAGE_H
