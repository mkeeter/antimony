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

    /*
     *  Clears the export worker and hides the button
     *  Returns true if visibility has changed.
     */
    bool clearWorker();

    /*
     *  Sets the worker and shows the button
     *  Returns true if visibility has changed.
     */
    bool setWorker(ExportWorker* w);

    void setWidth(float width);

    /*
     *  Overload paint to show hover
     */
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* o,
               QWidget* w) override;

protected:
    /*
     *  On hover, highlight the button
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /*
     *  On mouse press, call run() if the worker is present
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    QScopedPointer<ExportWorker> worker;
    bool hover;
};
