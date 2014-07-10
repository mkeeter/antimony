#include <boost/python.hpp>

#include <QDebug>
#include <QTime>

#include "render/render_worker.h"
#include "cpp/shape.h"

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
    qDebug() << "Starting render";
    extract<Shape*> get_shape(shape);

    Q_ASSERT(get_shape.check());
    Shape* s = get_shape();

    qDebug() << s->math.c_str();

    QTime dieTime= QTime::currentTime().addSecs(1);
    while(QTime::currentTime() < dieTime);

    qDebug() << "Render done";
    emit(finished());
}
