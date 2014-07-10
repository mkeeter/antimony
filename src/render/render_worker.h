#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <Python.h>
#include <QObject>
#include <QMatrix4x4>

class RenderWorker : public QObject
{
    Q_OBJECT
public:
    explicit RenderWorker(PyObject* s, QMatrix4x4 m);
    ~RenderWorker();

public slots:
    void render();
signals:
    void finished();
protected:
    PyObject* shape;
    QMatrix4x4 matrix;
};

#endif // RENDER_WORKER_H
