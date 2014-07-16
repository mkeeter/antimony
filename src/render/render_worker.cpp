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

RenderWorker::RenderWorker(PyObject *s, QMatrix4x4 m)
    : QObject(NULL), shape(s), matrix(m)
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

    QMatrix4x4 m = matrix;
    QMatrix4x4 mf = m.inverted();
    QMatrix4x4 mi = mf.inverted();

    Q_ASSERT(mf(0, 3) == 0 && mf(1, 3) == 0 && mf(2, 3) == 0 && mf (3, 3) == 1);

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

    image = new RenderImage(&transformed);
    image->moveToThread(QApplication::instance()->thread());

    emit(finished());
}
