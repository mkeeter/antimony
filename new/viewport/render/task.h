#pragma once

#include <Python.h>
#include <QObject>
#include <QMatrix4x4>

class RenderTask : public QObject
{
Q_OBJECT
public:
    RenderTask(PyObject* s, QMatrix4x4 M);
    ~RenderTask();

protected:
    PyObject* shape;
    QMatrix4x4 M;
};
