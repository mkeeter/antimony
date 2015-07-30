#pragma once

#include <Python.h>

#include <QGraphicsTextItem>
#include <QScopedPointer>

#include "export/export_worker.h"

class InspectorExportButton : public QGraphicsTextItem
{
    Q_OBJECT
public:
    InspectorExportButton(QGraphicsItem* parent);
    void clearWorker();
    void setWorker(ExportWorker* w);
    bool hasWorker() const { return !worker.isNull(); }
    void setWidth(float width);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* o,
               QWidget* w) override;

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    QScopedPointer<ExportWorker> worker;
    bool hover;
};
