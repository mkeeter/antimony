#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "graph/datum/datum.h"
#include "ui/inspector/inspector_button.h"
#include "ui/inspector/inspector.h"
#include "ui/colors.h"

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
    painter->setPen(d->getValid() ? Colors::base02 : Colors::red);
    painter->setBrush(Colors::base03);

    setDefaultTextColor(hover ? Colors::base05 : Colors::base04);
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);
}
