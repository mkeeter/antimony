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

Control::Control(Node* node, QObject* parent)
    : QObject(parent), node(node)
{
    if (node)
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

void Control::watchDatums(QVector<QString> datums)
{
    for (auto n : datums)
    {
        Datum* d = node->getDatum(n);
        Q_ASSERT(d);
        connect(d, &Datum::changed, this, &Control::redraw);
        if (auto e = dynamic_cast<EvalDatum*>(d))
            watched[e] = "";
    }
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
    if (parent())
        dynamic_cast<Control*>(parent())->deleteNode(text);
    else
        App::instance()->pushStack(new UndoDeleteMultiCommand({node}, {}));
}

void Control::beginDrag()
{
    for (auto d=watched.begin(); d != watched.end(); ++d)
    {
        watched[d.key()] = d.key()->getExpr();
    }
}

void Control::endDrag()
{
    bool started = false;
    for (auto d=watched.begin(); d != watched.end(); ++d)
        if (watched[d.key()] != d.key()->getExpr())
        {
            if (!started)
            {
                App::instance()->beginUndoMacro("'drag'");
                started = true;
            }
            App::instance()->pushStack(
                    new UndoChangeExprCommand(
                        d.key(), watched[d.key()], d.key()->getExpr()));
        }

    if (started)
        App::instance()->endUndoMacro();
}

void Control::dragValue(QString name, double delta)
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    FloatDatum* f = dynamic_cast<FloatDatum*>(d);
    Q_ASSERT(f);

    f->dragValue(delta);
}

void Control::setValue(QString name, double new_value)
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    FloatDatum* f = dynamic_cast<FloatDatum*>(d);
    Q_ASSERT(f);

    bool ok = false;
    f->getExpr().toFloat(&ok);
    if (ok)
        f->setExpr(QString::number(new_value));
}

QColor Control::defaultPenColor() const
{
    return Colors::base04;
}

QColor Control::defaultBrushColor() const
{
    return Colors::dim(defaultPenColor());
}

void Control::setDefaultPen(bool highlight, QPainter *painter) const
{
    if (highlight)
        painter->setPen(QPen(Colors::highlight(defaultPenColor()), 2));
    else
        painter->setPen(QPen(defaultPenColor(), 2));
}

void Control::setDefaultBrush(bool highlight, QPainter *painter) const
{
    if (highlight)
        painter->setBrush(QBrush(Colors::highlight(defaultBrushColor())));
    else
        painter->setBrush(QBrush(defaultBrushColor()));
}

