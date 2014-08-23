#include <boost/python.hpp>
#include <boost/format.hpp>

#include <QApplication>
#include <QDebug>
#include <QTime>

#include "render/render_worker.h"
#include "render/render_image.h"

#include "cpp/shape.h"
#include "cpp/transform.h"

using namespace boost::python;

RenderWorker::RenderWorker(PyObject *s, QMatrix4x4 matrix)
    : QObject(NULL), shape(s), matrix(matrix), image(NULL)
{
    Py_INCREF(shape);
}

RenderWorker::~RenderWorker()
{
    Py_DECREF(shape);
}

void RenderWorker::render()
{
    extract<Shape> get_shape(shape);

    Q_ASSERT(get_shape.check());
    Shape s = get_shape();

    Q_ASSERT(!(isinf(s.bounds.zmin) ^ isinf(s.bounds.zmax)));

    if (!isinf(s.bounds.xmin) && !isinf(s.bounds.xmax) &&
        !isinf(s.bounds.xmin) && !isinf(s.bounds.xmax))
    {
        if (isinf(s.bounds.zmin))
        {
            // No 2D rendering for now.
        }
        else
        {
            render3d(s);
        }
    }

    emit(finished());
}

void RenderWorker::render3d(Shape s)
{
    QMatrix4x4 mf = matrix.inverted();
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

    Shape transformed = s.map(T);

    image = new RenderImage(transformed.bounds);
    image->render(&transformed);
    image->moveToThread(QApplication::instance()->thread());
}

void RenderWorker::render2d(Shape s)
{
    /* NO ADMITTANCE BEYOND THIS POINT UNTIL SEWER PIPE IS REPAIRED */
    /*
    QMatrix4x4 mf = m2d.inverted();
    QMatrix4x4 mi = mf.inverted();

    Transform T(
                (boost::format("+*Xf%g*Yf%g") %
                    mf(0,0) % mf(0,1)).str(),
                (boost::format("+*Xf%g*Yf%g") %
                    mf(1,0) % mf(1,1)).str(),
                "Z",
                (boost::format("+*Xf%g*Yf%g") %
                    mi(0,0) % mi(0,1)).str(),
                (boost::format("+*Xf%g*Yf%g") %
                    mi(1,0) % mi(1,1)).str(),
                "Z");

    Shape transformed = s.map(T);
    transformed.bounds.zmin = 0;
    transformed.bounds.zmax = 1;

    image = new RenderImage(transformed.bounds);
    image->render(&transformed);
    image->moveToThread(QApplication::instance()->thread());

    // Figure out the z bounds for the image using the full transform
    // (not the fake y-scaling transform)
    Bounds bz = Bounds(s.bounds.xmin, s.bounds.ymin, 0,
                       s.bounds.xmax, s.bounds.ymax, 1);
    Bounds bzt =  bz.map(Transform(
                         "X", "Y", "Z",
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    m3d(0,0) % m3d(0,1) % m3d(0,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    m3d(1,0) % m3d(1,1) % m3d(1,2)).str(),
                (boost::format("++*Xf%g*Yf%g*Zf%g") %
                    m3d(2,0) % m3d(2,1) % m3d(2,2)).str()));

    image->setZ(bzt.zmin, bzt.zmax);
    if (m3d(1,2))
    {
        image->applyGradient(m3d(2,2) > 0);
    }
    */
}
