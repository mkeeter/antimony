#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "datum/datum.h"
#include "ui/inspector/inspector_button.h"
#include "ui/inspector/inspector.h"

DatumTextButton::DatumTextButton(Datum *datum, QString label, QGraphicsItem *parent)
    : QGraphicsTextItem(parent), d(datum), hover(false)
{
    setHtml("<center>" + label + "</center>");
    setTextWidth(150);
    connect(d, SIGNAL(changed()), this, SLOT(redraw()));
}

void DatumTextButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = true;
    update();
}

void DatumTextButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = false;
    update();
}

void DatumTextButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit(pressed(d));
    }
}

void DatumTextButton::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *o,
                             QWidget *w)
{
    painter->setBrush(d->getValid() ? Qt::white : QColor("#faa"));
    if (hover)
    {
        painter->setPen(QPen(QColor(150, 150, 150), 3));
    }
    else
    {
        painter->setPen(Qt::NoPen);
    }
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);
}
