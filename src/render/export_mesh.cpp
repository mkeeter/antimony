#include <Python.h>
#include <QDebug>
#include <QTime>

#include "render/export_mesh.h"

#include "fab/types/shape.h"
#include "fab/formats/stl.h"
#include "fab/tree/triangulate.h"
#include "fab/util/region.h"

ExportMeshWorker::ExportMeshWorker(Shape s, float resolution,
                                   bool detect_edges, QString filename)
    : QObject(), shape(s), resolution(resolution),
      detect_edges(detect_edges), filename(filename)
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

    QTime time;
    time.start();
    triangulate(shape.tree.get(), r, detect_edges, &verts, &count);
    qDebug() << time.elapsed();

    save_stl(verts, count, filename.toStdString().c_str());
    free_arrays(&r);
    free(verts);

    qDebug() << "Done.";
    emit(finished());
}
