#ifndef RENDER_IMAGE_H
#define RENDER_IMAGE_H

#include <QObject>
#include <QImage>
#include <QGraphicsPixmapItem>
#include <QVector3D>
#include <QColor>

#include <cstdint>

#include "fab/types/bounds.h"

struct Shape;
class Viewport;
class DepthImageItem;

class RenderImage : public QObject
{
    Q_OBJECT
public:
    explicit RenderImage(Bounds b, QVector3D pos, float scale);
    void render(Shape* shape);
    void applyGradient(bool direction);

    DepthImageItem* addToViewport(Viewport* viewport);

    /** Sets the normals in the shaded image.
     *  (used for 2D image shading).
     */
    void setNormals(float xy, float z);

    void setColor(QColor color_) { color = color_; }

    void setFlat(bool f) { flat = f; }

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

    /** Used to halt render operations; set by the halt() slot. */
    int halt_flag;

    QColor color;
    bool flat;

    friend class RenderTask;
};

#endif // RENDER_IMAGE_H
