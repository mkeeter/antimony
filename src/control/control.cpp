#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QRegularExpression>

#include "control/control.h"

#include "ui/viewport/viewport.h"
#include "ui/colors.h"

#include "graph/node/node.h"
#include "graph/datum/datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/script_datum.h"

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

void Control::deleteNode()
{
    if (parent())
        dynamic_cast<Control*>(parent())->deleteNode();
    else
        node->deleteLater();
}

void Control::dragValue(QString name, double delta)
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    FloatDatum* f = dynamic_cast<FloatDatum*>(d);
    Q_ASSERT(f);

    bool ok = false;
    QString s = f->getExpr();
    double v = s.toFloat(&ok);
    if (ok)
    {
        f->setExpr(QString::number(v + delta));
        return;
    }

    QRegularExpression regex(
        "(.*[+\\-]\\s*)(\\d*(\\.\\d*|)(e\\d+(\\.\\d*|)|))"
    );
    auto match= regex.match(s);
    if (match.isValid())
    {
        v = match.captured(2).toFloat(&ok);
        if (ok)
        {
            f->setExpr(match.captured(1) +
                       QString::number(v + delta));
        }
    }
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

