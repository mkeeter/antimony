#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include "ui/canvas/inspector/inspector_export.h"
#include "ui/util/colors.h"

InspectorExportButton::InspectorExportButton(QGraphicsItem* parent)
    : QGraphicsTextItem(parent), hover(false)
{
    setDefaultTextColor(Colors::base05);

    setHtml("<center>Export</center>");
    setToolTip("Export");
    setAcceptHoverEvents(true);
    hide();
}

void InspectorExportButton::clearWorker()
{
    if (worker)
        worker.reset();

    if (isVisible())
        hide();
}

void InspectorExportButton::setWidth(float width)
{
    this->setTextWidth(width);
}

void InspectorExportButton::setWorker(ExportWorker* w)
{
    worker.reset(w);
    if (!isVisible())
        show();
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
