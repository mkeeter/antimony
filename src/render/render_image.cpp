#include <Python.h>

#include "render/render_image.h"
#include "ui/canvas.h"
#include "cpp/shape.h"

RenderImage::RenderImage(Shape* shape, QObject* parent)
    : QObject(parent), bounds(shape->bounds),
      image(shape->bounds.xmax - shape->bounds.xmin,
            shape->bounds.ymax - shape->bounds.ymin,
            QImage::Format_RGB32)
{
    image.fill(0xff);
}

RenderImage::~RenderImage()
{
    for (auto p = pixmaps.begin(); p != pixmaps.end(); ++p)
    {
        p.key()->scene->removeItem(p.value());
        delete p.value();
    }
}

void RenderImage::addToCanvas(Canvas *canvas)
{
    QGraphicsPixmapItem* pix = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    pix->setPos(bounds.xmin, bounds.ymin);
    canvas->scene->addItem(pix);
    pixmaps[canvas] = pix;
}
