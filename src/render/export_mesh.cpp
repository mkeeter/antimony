#include <QDebug>

#include "render/export_mesh.h"
#include "cpp/shape.h"

#include "tree/triangulate.h"
#include "util/region.h"

ExportMeshWorker::ExportMeshWorker(Shape s, float resolution)
    : QObject(), shape(s), resolution(resolution)
{
    qDebug() << shape.math.c_str();
    float dx = (shape.bounds.xmax - shape.bounds.xmin) / 20;
    float dy = (shape.bounds.ymax - shape.bounds.ymin) / 20;
    float dz = (shape.bounds.zmax - shape.bounds.zmin) / 20;
    qDebug() << dx << dy << dz;
    // Nothing to do here
}

void ExportMeshWorker::render()
{
    float* verts;
    unsigned count;

    // Add a little bit of padding to the render bounds
    float dx = (shape.bounds.xmax - shape.bounds.xmin) / 20;
    float dy = (shape.bounds.ymax - shape.bounds.ymin) / 20;
    float dz = (shape.bounds.zmax - shape.bounds.zmin) / 20;
    qDebug() << dx << dy << dz;

    Region r = (Region){
        .imin=0, .jmin=0, .kmin=0,
        .ni=uint32_t((shape.bounds.xmax - shape.bounds.xmin) * resolution),
        .nj=uint32_t((shape.bounds.ymax - shape.bounds.ymin) * resolution),
        .nk=uint32_t((shape.bounds.zmax - shape.bounds.zmin) * resolution),
    };
    r.voxels = r.ni * r.nj * r.nk;

    build_arrays(
            &r, shape.bounds.xmin - dx,
                shape.bounds.ymin - dy,
                shape.bounds.zmin - dz,
                shape.bounds.xmax + dx,
                shape.bounds.ymax + dy,
                shape.bounds.zmax + dz);

    triangulate(shape.tree.get(), r, &verts, &count);

    free_arrays(&r);
    free(verts);
    qDebug() << "Done.";
    emit(finished());
}
