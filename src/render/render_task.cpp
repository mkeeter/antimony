#include <boost/python.hpp>
#include <boost/format.hpp>

#include <QApplication>
#include <QDebug>
#include <QTime>

#include "render/render_task.h"
#include "render/render_image.h"

#include "fab/types/shape.h"
#include "fab/types/transform.h"

using namespace boost::python;

RenderTask::RenderTask(PyObject *s, QMatrix4x4 matrix,
                       float scale, int refinement)
    : QObject(NULL), shape(s), matrix(matrix),
      scale(scale), refinement(refinement), image(NULL),
      is_empty(false)
{
    Py_INCREF(shape);
}

RenderTask::~RenderTask()
{
    if (image)
        delete image;
    Py_DECREF(shape);
}

RenderTask* RenderTask::getNext() const
{
    return refinement
        ? new RenderTask(shape, matrix, scale*2, refinement - 1)
        : NULL;
}

bool RenderTask::hasFinishedRender() const
{
    // Checks that the rendering was completed
    // (rather than halted mid-render)
    return image && (image->halt_flag == 0);
}

DepthImageItem* RenderTask::getDepthImage(Viewport* viewport)
{
    Q_ASSERT(image);

    auto d = image->addToViewport(viewport);
    delete image;
    image = NULL;

    return d;
}

void RenderTask::render()
{
    QTime timer;
    timer.start();

    extract<Shape> get_shape(shape);

    Q_ASSERT(get_shape.check());
    Shape s = get_shape();

    if (!isinf(s.bounds.xmin) && !isinf(s.bounds.xmax) &&
        !isinf(s.bounds.xmin) && !isinf(s.bounds.xmax))
    {
        if (isinf(s.bounds.zmin) || isinf(s.bounds.zmax))
            render2d(s);
        else
            render3d(s);
        image->moveToThread(QApplication::instance()->thread());
    }
    else
    {
        is_empty = true;
    }

    time_taken = timer.elapsed();
    emit(finished());
}

void RenderTask::render3d(Shape s)
{
    Transform T = getTransform(matrix);
    Shape transformed = s.map(T);

    image = new RenderImage(
            transformed.bounds,
            matrix.inverted() * QVector3D(
                (transformed.bounds.xmin + transformed.bounds.xmax)/2,
                (transformed.bounds.ymin + transformed.bounds.ymax)/2,
                (transformed.bounds.zmin + transformed.bounds.zmax)/2),
            scale);
    connect(this, &RenderTask::halt, image, &RenderImage::halt);
    image->render(&transformed);

    if (s.r != -1 && s.g != -1 && s.g != -1)
        image->setColor(QColor(s.r, s.g, s.b));
}

void RenderTask::render2d(Shape s)
{
    QMatrix4x4 matrix_flat = matrix;
    matrix_flat(0, 2) = 0;
    matrix_flat(1, 2) = 0;
    matrix_flat(2, 0) = 0;
    matrix_flat(2, 1) = 0;
    matrix_flat(2, 2) = 1;

    Shape s_flat(s.math, s.bounds.xmin, s.bounds.ymin, 0,
                         s.bounds.xmax, s.bounds.ymax, 0);

    Transform T_flat = getTransform(matrix_flat);
    Shape transformed = s_flat.map(T_flat);

    // Render the flattened shape, but with bounds equivalent to the shape's
    // position in a 3D bounding box.
    Bounds b3d = Bounds(s.bounds.xmin, s.bounds.ymin, 0,
                        s.bounds.xmax, s.bounds.ymax, 0.0001).
                 map(getTransform(matrix));
    image = new RenderImage(
            b3d,
            matrix.inverted() *
                QVector3D((b3d.xmin + b3d.xmax)/2,
                          (b3d.ymin + b3d.ymax)/2,
                          (b3d.zmin + b3d.zmax)/2),
            scale);
    connect(this, &RenderTask::halt, image, &RenderImage::halt);
    image->render(&transformed);

    if (matrix(1,2))
        image->applyGradient(matrix(2,2) > 0);

    if (s.r != -1 && s.g != -1 && s.g != -1)
        image->setColor(QColor(s.r, s.g, s.b));

    image->setNormals(
            sqrt(pow(matrix(0,2),2) + pow(matrix(1,2),2)),
            fabs(matrix(2,2)));
    image->setFlat(true);
}

Transform RenderTask::getTransform(QMatrix4x4 m)
{
    QMatrix4x4 mf = m.inverted();
    QMatrix4x4 mi = mf.inverted();

    Transform T = Transform(
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    mf(0,0) % mf(0,1) % mf(0,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    mf(1,0) % mf(1,1) % mf(1,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    mf(2,0) % mf(2,1) % mf(2,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    mi(0,0) % mi(0,1) % mi(0,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    mi(1,0) % mi(1,1) % mi(1,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    mi(2,0) % mi(2,1) % mi(2,2)).str());

    return T;
}
