#include <Python.h>

#include "render/render_image.h"
#include "ui/canvas.h"

#include "cpp/shape.h"
#include "util/region.h"
#include "tree/render.h"

#include "formats/png.h"

RenderImage::RenderImage(Shape* shape, QObject* parent)
    : QObject(parent), bounds(shape->bounds),
      image(shape->bounds.xmax - shape->bounds.xmin,
            shape->bounds.ymax - shape->bounds.ymin,
            QImage::Format_RGB32)
{
    render(shape);
}

RenderImage::~RenderImage()
{
    for (auto p = pixmaps.begin(); p != pixmaps.end(); ++p)
    {
        p.key()->scene->removeItem(p.value());
        delete p.value();
    }
}

void RenderImage::render(Shape *shape)
{
    image.fill(0x000000);

    uint8_t* pixels8(new uint8_t[image.width() * image.height()]);
    uint8_t** image8(new uint8_t*[image.height()]);
    for (int i=0; i < image.height(); ++i)
    {
        image8[i] = pixels8 + (image.width() * i);
    }
    memset(pixels8, 0, image.width() * image.height());


    Region r = (Region) {
            .imin=0, .jmin=0, .kmin=0,
            .ni=(uint32_t)image.width(), .nj=(uint32_t)image.height(),
            .nk=uint32_t(shape->bounds.zmax - shape->bounds.zmin)
    };

    build_arrays(&r, shape->bounds.xmin, shape->bounds.ymin, shape->bounds.zmin,
                     shape->bounds.xmax, shape->bounds.ymax, shape->bounds.zmax);
    int halt=0;
    render8(shape->tree.get(), r, image8, &halt);
    qDebug() << r.Z[r.nk] << r.Z[0];

    free_arrays(&r);

    for (int j=0; j < image.height(); ++j)
    {
        for (int i=0; i < image.width(); ++i)
        {
            if (image8[j][i])
            {
                image.setPixel(i, image.height() - j - 1,
                               (image8[j][i] << 16) |
                               (image8[j][i] <<  8) |
                                image8[j][i]);
            }
            else
            {
                image.setPixel(i, image.height() - j - 1, 0xff0000);
            }
        }
    }

    delete [] pixels8;
    delete [] image8;
}

void RenderImage::addToCanvas(Canvas *canvas)
{
    QGraphicsPixmapItem* pix = new QGraphicsPixmapItem(QPixmap::fromImage(image));
    pix->setPos(bounds.xmin, -bounds.ymax);
    canvas->scene->addItem(pix);
    pixmaps[canvas] = pix;
}
