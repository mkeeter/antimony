#include <Python.h>

#include <QtConcurrent>
#include <QThread>

#include "viewport/render/task.h"
#include "viewport/render/instance.h"

RenderTask::RenderTask(RenderInstance* parent, PyObject* s, QMatrix4x4 M)
    : shape(s), M(M)
{
    Py_INCREF(shape);

    auto future = QtConcurrent::run(this, &RenderTask::async);
    QFutureWatcher<void> watcher;
    watcher.setFuture(future);

    connect(&watcher, &decltype(watcher)::finished,
            parent, &RenderInstance::onTaskFinished);
}

RenderTask::~RenderTask()
{
    Py_DECREF(shape);
}

void RenderTask::halt()
{
    // Nothing to do here
}

void RenderTask::async()
{
    for (int i=0; i < 5; ++i)
    {
        qDebug() << i;
        QThread::sleep(1);
    }
}
