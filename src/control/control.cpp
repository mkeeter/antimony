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
    : QObject(), node(node), glow(false)
{
    connect(node, &Node::destroyed, this, &Control::deleteLater);
}

QPainterPath Control::shape(QMatrix4x4 m, QMatrix4x4 t) const
{
    QPainterPath p;
    p.addRect(bounds(m, t));
    return p;
}

Node* Control::getNode() const
{
    return node;
}

double Control::getValue(QString name) const
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    double v = PyFloat_AsDouble(d->getValue());
    Q_ASSERT(!PyErr_Occurred());

    return v;
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
    for (auto d : node->findChildren<EvalDatum*>(QString(),
                Qt::FindDirectChildrenOnly))
        datums[d] = d->getExpr();
}

void Control::drag(QVector3D center)
{
    // Call Python function here
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
