#include <Python.h>

#include <QMatrix4x4>

#include "viewport/control/control.h"
#include "viewport/control/control_instance.h"
#include "viewport/view.h"

#include "graph/proxy/node.h"
#include "graph/node.h"

#include "app/app.h"
#include "undo/undo_delete_multi.h"
#include "undo/undo_change_expr.h"

Control::Control(NodeProxy* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void Control::makeInstanceFor(ViewportView* v)
{
    auto i = new ControlInstance(this, v);
    connect(this, &QObject::destroyed, i, &QObject::deleteLater);
    connect(this, &Control::redraw, i, &ControlInstance::redraw);

    // Echo onZoomTo and onFocus signals up from instances
    connect(i, &ControlInstance::onFocus, this, &Control::onFocus);
    connect(i, &ControlInstance::onZoomTo, this, &Control::onZoomTo);
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

void Control::beginDrag()
{
    // Store all datum expressions so that we can make an undo action
    // that undoes the upcoming drag operation.
    datum_text.clear();
    auto node = static_cast<NodeProxy*>(parent())->getMutableNode();
    for (auto d : node->childDatums())
        datum_text[d] = QString::fromStdString(d->getText());
}

void Control::endDrag()
{
    bool started = false;
    for (auto d=datum_text.begin(); d != datum_text.end(); ++d)
    {
        auto expr = QString::fromStdString(d.key()->getText());
        if (datum_text[d.key()] != expr)
        {
            if (!started)
            {
                App::instance()->beginUndoMacro("'drag'");
                started = true;
            }
            App::instance()->pushUndoStack(
                    new UndoChangeExpr(
                        d.key(), datum_text[d.key()], expr));
        }
    }

    if (started)
        App::instance()->endUndoMacro();
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

QString Control::getName() const
{
    return QString::fromStdString(
            static_cast<NodeProxy*>(parent())->getNode()->getName());
}

const Node* Control::getNode() const
{
    return static_cast<NodeProxy*>(parent())->getNode();
}

void Control::setFocus(bool focus)
{
    if (focus != has_focus)
    {
        has_focus = focus;
        emit(redraw());
    }
}
