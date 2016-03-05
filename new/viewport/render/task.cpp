#include "viewport/render/task.h"

RenderTask::RenderTask(PyObject* s, QMatrix4x4 M)
    : shape(s), M(M)
{
    Py_INCREF(shape);
}

RenderTask::~RenderTask()
{
    Py_DECREF(shape);
}
