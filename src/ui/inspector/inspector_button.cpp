#include <Python.h>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "app.h"
#include "datum/datum.h"
#include "datum/script_datum.h"
#include "ui/main_window.h"
#include "ui/inspector/inspector_button.h"

DatumTextButton::DatumTextButton(Datum *datum, QString label, QGraphicsItem *parent)
    : QGraphicsTextItem(parent), d(datum), hover(false), background(Qt::white)
{
    setHtml("<center>" + label + "</center>");
    setTextWidth(150);
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
        App::instance()->getWindow()->openScript(
                    dynamic_cast<ScriptDatum*>(d));
    }
}

void DatumTextButton::paint(QPainter *painter,
                             const QStyleOptionGraphicsItem *o,
                             QWidget *w)
{
    painter->setBrush(background);
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
