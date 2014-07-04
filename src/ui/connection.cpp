#include <Python.h>

#include "ui/connection.h"
#include "ui/canvas.h"
#include "ui/colors.h"

#include "datum/datum.h"
#include "datum/link.h"

#include "node/node.h"

#include "control/control.h"

Connection::Connection(Link* link, Canvas* canvas)
    : QGraphicsObject(), link(link), canvas(canvas)
{
    setFlags(QGraphicsItem::ItemIsSelectable);
    canvas->scene->addItem(this);
    setZValue(2);
}

QRectF Connection::boundingRect() const
{
    return path().boundingRect();
}

QPainterPath Connection::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    return stroker.createStroke(path());
}

QPointF Connection::startPos() const
{
    Datum* d = dynamic_cast<Datum*>(link->parent());
    Control* c = canvas->getControl(dynamic_cast<Node*>(d->parent()));
    return c->datumOutputPosition(d);
}

QPointF Connection::endPos() const
{
    Datum* d = dynamic_cast<Datum*>(link->parent());
    Control* c = canvas->getControl(dynamic_cast<Node*>(d->parent()));
    return c->datumInputPosition(d);
}

QPainterPath Connection::path() const
{
    QPointF start = startPos();
    QPointF end = endPos();

    float length = 50;
    if (end.x() <= start.x())
    {
        length += (start.x() - end.x()) / 2;
    }

    QPainterPath p;
    p.moveTo(start);
    p.cubicTo(start + QPointF(length, 0),
              end - QPointF(length, 0), end);
   return p;
}

void Connection::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color = Colors::getColor(dynamic_cast<Datum*>(link->parent()));
    if (isSelected())
    {
        color = Colors::highlight(color);
    }

    painter->setPen(QPen(color, 4));
    painter->drawPath(path());
}
