#include <Python.h>

#include "control/control.h"

#include "ui/viewport/viewport.h"
#include "ui/util/colors.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/script_datum.h"

#include "app/app.h"
#include "app/undo/undo_change_expr.h"
#include "app/undo/undo_delete_multi.h"

Control::Control(Node* node)
    : QObject(), node(node), glow(false), drag_func(NULL)
{
    connect(node, &Node::clearControlTouchedFlag,
            this, &Control::clearTouchedFlag);
    connect(node, &Node::deleteUntouchedControls,
            this, &Control::deleteIfNotTouched);

    connect(node, &Node::destroyed, this, &Control::deleteLater);
}

Control::~Control()
{
    Py_XDECREF(drag_func);
}

QRectF Control::bounds(QMatrix4x4 m) const
{
    // Default implementation is the bounding rect of shape().
    return shape(m).boundingRect();
}

Node* Control::getNode() const
{
    return node;
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
    for (auto d : node->findChildren<EvalDatum*>(
                QString(), Qt::FindDirectChildrenOnly))
        datums[d] = d->getExpr();
}

void Control::drag(QVector3D center)
{
    auto p = node->proxy();
    auto x = PyFloat_FromDouble(center.x());
    auto y = PyFloat_FromDouble(center.y());
    auto z = PyFloat_FromDouble(center.z());

    PyObject_CallFunctionObjArgs(p, x, y, z);

    Py_DECREF(p);
    Py_DECREF(x);
    Py_DECREF(y);
    Py_DECREF(z);
}

void Control::endDrag()
{
    bool started = false;
    for (auto d=datums.begin(); d != datums.end(); ++d)
        if (datums[d.key()] != d.key()->getExpr())
        {
            if (!started)
            {
                App::instance()->beginUndoMacro("'drag'");
                started = true;
            }
            App::instance()->pushStack(
                    new UndoChangeExprCommand(
                        d.key(), datums[d.key()], d.key()->getExpr()));
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
