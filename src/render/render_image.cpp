#include <Python.h>

#include <QCoreApplication>

#include "render/render_image.h"
#include "ui/viewport/depth_image.h"

#include "fab/types/shape.h"
#include "fab/util/region.h"
#include "fab/tree/render.h"
#include "fab/formats/png.h"

RenderImage::RenderImage(Bounds b, QVector3D pos, float scale)
    : QObject(), bounds(b), pos(pos), scale(scale),
      depth((b.xmax - b.xmin) * scale,
            (b.ymax - b.ymin) * scale,
            QImage::Format_RGB32),
      shaded(depth.width(), depth.height(), depth.format()),
      halt_flag(0), color(255, 255, 255), flat(false)
{
    // Nothing to do here
    // (render() must be called explicity)
}

void RenderImage::halt()
{
    halt_flag = true;
}

static void processEvents()
{
    QCoreApplication::processEvents();
}

void RenderImage::render(Shape *shape)
{
    depth.fill(0x000000);

    uint16_t* d16(new uint16_t[depth.width() * depth.height()]);
    uint16_t** d16_rows(new uint16_t*[depth.height()]);
    uint8_t (*s8)[3] = new uint8_t[depth.width() * depth.height()][3];
    uint8_t (**s8_rows)[3] = new decltype(s8)[depth.height()];

    for (int i=0; i < depth.height(); ++i)
    {
        d16_rows[i] = &d16[depth.width() * i];
        s8_rows[i] = &s8[depth.width() * i];
    }
    memset(d16, 0, depth.width() * depth.height() * 2);
    memset(s8, 0, depth.width() * depth.height() * 3);

    Region r = (Region) {
            .imin=0, .jmin=0, .kmin=0,
            .ni=(uint32_t)depth.width(), .nj=(uint32_t)depth.height(),
            .nk=uint32_t(fmax(1, (shape->bounds.zmax -
                                  shape->bounds.zmin) * scale))
    };

    build_arrays(&r, shape->bounds.xmin, shape->bounds.ymin, shape->bounds.zmin,
                     shape->bounds.xmax, shape->bounds.ymax, shape->bounds.zmax);
    render16(shape->tree.get(), r, d16_rows, &halt_flag, &processEvents);
    shaded8(shape->tree.get(), r, d16_rows, s8_rows, &halt_flag, &processEvents);

    free_arrays(&r);

    // Copy from bitmap arrays into a QImage
    for (int j=0; j < depth.height(); ++j)
    {
        for (int i=0; i < depth.width(); ++i)
        {
            uint8_t pix = d16_rows[j][i] >> 8;
            uint8_t* norm = s8_rows[j][i];
            if (pix)
            {
                depth.setPixel(i, depth.height() - j - 1,
                               pix | (pix << 8) | (pix << 16));
                shaded.setPixel(i, depth.height() - j - 1,
                        norm[0] | (norm[1] << 8) | (norm[2] << 16));
            }
        }
    }

    delete [] s8;
    delete [] s8_rows;
    delete [] d16;
    delete [] d16_rows;

}

void RenderImage::applyGradient(bool direction)
{
    for (int j=0; j < depth.height(); ++j)
    {
        for (int i=0; i < depth.width(); ++i)
        {
            uint8_t pix = depth.pixel(i, j) & 0xff;
            if (pix)
            {
                if (direction)
                    pix *= j / float(depth.height());
                else
                    pix *= 1 - j / float(depth.height());
                depth.setPixel(i, j, pix | (pix << 8) | (pix << 16));
            }
        }
    }
}

void RenderImage::setNormals(float xy, float z)
{
    shaded.fill((int(z * 255) << 16) | int(xy * 255));
}

DepthImageItem* RenderImage::addToViewport(Viewport* viewport)
{
    return new DepthImageItem(pos,
            QVector3D(bounds.xmax - bounds.xmin,
                      bounds.ymax - bounds.ymin,
                      bounds.zmax - bounds.zmin),
            depth, shaded, color, flat, viewport);
}

