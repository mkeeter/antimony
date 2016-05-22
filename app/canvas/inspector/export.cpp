#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "canvas/inspector/export.h"
#include "app/colors.h"

InspectorExportButton::InspectorExportButton(QGraphicsItem* parent)
    : QGraphicsTextItem(parent), hover(false)
{
    setDefaultTextColor(Colors::base05);

    setHtml("<center>Export</center>");
    setToolTip("Export");
    setAcceptHoverEvents(true);

    hide();
}

bool InspectorExportButton::clearWorker()
{
    if (worker)
    {
        worker.reset();
    }

    if (isVisible())
    {
        hide();
        return true;
    }
    return false;
}

void InspectorExportButton::setWidth(float width)
{
    this->setTextWidth(width);
}

bool InspectorExportButton::setWorker(ExportWorker* w)
{
    worker.reset(w);
    if (!isVisible())
    {
        show();
        return true;
    }
    return false;
}

void InspectorExportButton::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = true;
    update();
}

void InspectorExportButton::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    hover = false;
    update();
}

void InspectorExportButton::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
    if (event->button() == Qt::LeftButton && worker)
        worker->run();
}

void InspectorExportButton::paint(
        QPainter* painter, const QStyleOptionGraphicsItem* o, QWidget* w)
{
    painter->setBrush(hover ? Colors::base03 : Colors::base02);
    painter->setPen(Qt::NoPen);
    painter->drawRect(boundingRect());
    QGraphicsTextItem::paint(painter, o, w);
}
