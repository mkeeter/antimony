#include <Python.h>

#include "control/control.h"

#include "ui/viewport/viewport.h"
#include "ui/util/colors.h"

#include "graph/node.h"
#include "graph/datum.h"
#include "graph/graph.h"

#include "app/app.h"
#include "app/undo/undo_change_expr.h"
#include "app/undo/undo_delete_multi.h"

Control::Control(Node* node, PyObject* drag_func)
    : QObject(), node(node), drag_func(drag_func), glow(false),
      touched(false), relative(true)
{
    // Nothing to do here
}

Control::~Control()
{
    Py_XDECREF(drag_func);
}

void Control::setDragFunc(PyObject* new_drag_func)
{
    Py_XDECREF(drag_func);
    drag_func = new_drag_func;
}

QRectF Control::bounds(QMatrix4x4 m) const
{
    // Default implementation is the bounding rect of shape().
    return shape(m).boundingRect();
}

void Control::deleteNode(QString text)
{
    App::instance()->pushStack(new UndoDeleteMultiCommand({node}, {}, text));
}

void Control::beginDrag()
{
    // Store all datum expressions so that we can make an undo action
    // that undoes the upcoming drag operation.
    datums.clear();
    for (auto d : node->childDatums())
        datums[d] = QString::fromStdString(d->getText());

    is_dragging = true;
}

void Control::drag(QVector3D center, QVector3D diff)
{
    auto p = node->mutableProxy();
    auto x = PyFloat_FromDouble(relative ? diff.x() : center.x());
    auto y = PyFloat_FromDouble(relative ? diff.y() : center.y());
    auto z = PyFloat_FromDouble(relative ? diff.z() : center.z());

    if (drag_func)
    {
        PyObject_CallFunctionObjArgs(drag_func, p, x, y, z, NULL);
        if (PyErr_Occurred())
        {
            PyErr_Print();
            PyErr_Clear();
        }
    }

    Py_DECREF(p);
    Py_DECREF(x);
    Py_DECREF(y);
    Py_DECREF(z);
}

void Control::endDrag()
{
    is_dragging = false;

    bool started = false;
    for (auto d=datums.begin(); d != datums.end(); ++d)
    {
        auto expr = QString::fromStdString(d.key()->getText());
        if (datums[d.key()] != expr)
        {
            if (!started)
            {
                App::instance()->beginUndoMacro("'drag'");
                started = true;
            }
            App::instance()->pushStack(
                    new UndoChangeExprCommand(
                        d.key(), datums[d.key()], expr));
        }
    }

    if (started)
        App::instance()->endUndoMacro();
}

void Control::setGlow(bool g)
{
    if (g != glow)
    {
        glow = g;
        emit(redraw());
    }
}

bool Control::checkTouched()
{
    const bool was_touched = touched;
    touched = false;
    return was_touched;
}
