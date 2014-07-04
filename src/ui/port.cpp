#include <Python.h>

#include <QPainter>

#include "ui/port.h"
#include "ui/inspector.h"

#include "datum/datum.h"

Port::Port(Datum* d, NodeInspector* inspector) :
    QGraphicsObject(inspector), datum(d), opacity(1)
{
    // Nothing to do here
}

QRectF Port::boundingRect() const
{
    return QRectF(0, 0, 10, 10);
}

void Port::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setBrush(QColor(255, 0, 0, opacity*255));
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
}
////////////////////////////////////////////////////////////////////////////////

InputPort::InputPort(Datum *d, NodeInspector *inspector)
    : Port(d, inspector)
{
    // Nothing to do here
}

OutputPort::OutputPort(Datum *d, NodeInspector *inspector)
    : Port(d, inspector)
{
    // Nothing to do here
}
