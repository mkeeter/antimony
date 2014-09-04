#include <QDebug>
#include <cmath>

#include "render/export_bitmap.h"

#include "fab/types/shape.h"
#include "fab/tree/render.h"
#include "fab/util/region.h"
#include "fab/formats/png.h"

ExportBitmapWorker::ExportBitmapWorker(Shape s, float resolution, QString filename)
    : QObject(), shape(s), resolution(resolution), filename(filename)
{
    qDebug() << shape.math.c_str();
    float dx = (shape.bounds.xmax - shape.bounds.xmin) / 20;
    float dy = (shape.bounds.ymax - shape.bounds.ymin) / 20;
    float dz = (shape.bounds.zmax - shape.bounds.zmin) / 20;
    qDebug() << dx << dy << dz;
    // Nothing to do here
}

void ExportBitmapWorker::render()
{
    // Add a little bit of padding to the render bounds
    float dx = (shape.bounds.xmax - shape.bounds.xmin) / 20;
    float dy = (shape.bounds.ymax - shape.bounds.ymin) / 20;
    float dz = 0;
    Region r =  (Region){
            .imin=0, .jmin=0, .kmin=0,
            .ni=uint32_t((shape.bounds.xmax - shape.bounds.xmin) * resolution),
            .nj=uint32_t((shape.bounds.ymax - shape.bounds.ymin) * resolution),
            .nk=1
    };

    if (!isinf(shape.bounds.zmin) &&
        !isinf(shape.bounds.zmax))
    {
        r.nk = uint32_t((shape.bounds.zmax - shape.bounds.zmin) * resolution);
        dz = (shape.bounds.zmax - shape.bounds.zmin) / 20;
    }

    build_arrays(
            &r, shape.bounds.xmin - dx,
                shape.bounds.ymin - dy,
                shape.bounds.zmin - dz,
                shape.bounds.xmax + dx,
                shape.bounds.ymax + dy,
                shape.bounds.zmax + dz);

    uint16_t* d16(new uint16_t[r.ni * r.nj]);
    uint16_t** d16_rows(new uint16_t*[r.nj]);

    for (unsigned i=0; i < r.nj; ++i)
    {
        d16_rows[i] = d16 + (r.ni * i);
    }

    memset(d16, 0, 2 * r.ni * r.nj);
    int halt_flag = 0;
    render16(shape.tree.get(), r, d16_rows, &halt_flag);

    float bounds[6] = {r.X[0], r.Y[0], r.Z[0],
                       r.X[r.ni], r.Y[r.nj], r.Z[r.nk]};

    // Flip rows before saving image
    for (unsigned i=0; i < r.nj; ++i)
    {
        d16_rows[r.nj - i - 1] = d16 + (r.ni * i);
    }
    save_png16L(filename.toStdString().c_str(), r.ni, r.nj,
                bounds, d16_rows);

    free_arrays(&r);
    delete [] d16;
    delete [] d16_rows;
    qDebug() << "Done.";
    emit(finished());
}
