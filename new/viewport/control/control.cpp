#include <Python.h>

#include <QMatrix4x4>

#include "viewport/control/control.h"
#include "viewport/control/instance.h"
#include "viewport/view.h"

#include "graph/proxy/node.h"
#include "graph/node.h"

#include "app/app.h"
#include "undo/undo_delete_multi.h"

Control::Control(NodeProxy* parent)
    : QObject(parent)
{
    // Nothing to do here
}

Control::~Control()
{
    for (auto i : instances)
    {
        disconnect(i, &QGraphicsObject::destroyed, 0, 0);
        i->deleteLater();
    }
}

void Control::makeInstanceFor(ViewportView* v)
{
    auto i = new ControlInstance(this, v);
    instances[v] = i;
    connect(i, &QObject::destroyed, [=]{ this->instances.remove(v); });
}

QRectF Control::bounds(QMatrix4x4 m) const
{
    // Default implementation is the bounding rect of shape().
    return shape(m).boundingRect();
}

void Control::setDragFunc(PyObject* new_drag_func)
{
    Py_XDECREF(drag_func);
    drag_func = new_drag_func;
}

void Control::drag(QVector3D center, QVector3D diff)
{
    auto node = static_cast<NodeProxy*>(parent())->getMutableNode();

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

    for (auto i : {p, x, y, z})
    {
        Py_DECREF(i);
    }
}

////////////////////////////////////////////////////////////////////////////////

void Control::deleteNode()
{
    auto node = static_cast<NodeProxy*>(parent())->getMutableNode();
    App::instance()->pushUndoStack(new UndoDeleteMulti({node}, {}, {}));
}
