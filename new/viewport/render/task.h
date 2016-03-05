#pragma once

#include <Python.h>

#include <QObject>
#include <QMatrix4x4>

class RenderInstance;

class RenderTask : public QObject
{
Q_OBJECT
public:
    RenderTask(RenderInstance* parent, PyObject* s, QMatrix4x4 M);
    ~RenderTask();

    /*
     *  Request that the rendering task halts early
     */
    void halt();

protected:
    /*
     *  Async rendering task
     */
    void async();

    PyObject* shape;
    QMatrix4x4 M;
};
