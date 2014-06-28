#ifndef POINT3D_CONTROL_H
#define POINT3D_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/control.h"

class Point3DControl : public Control
{
    Q_OBJECT
public:
    explicit Point3DControl(Canvas* canvas, Node* node);

    /** Returns the bounding rect in screen coordinates.
     */
    virtual QRectF boundingRect() const override;

    /** Paints a single circle.
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

    /** Returns a painter path shape for selection.
     */
    virtual QPainterPath shape() const override;

    /** Drags the point in x, y, and z.
     */
    virtual void drag(QVector3D center, QVector3D delta) override;

    virtual QPointF inspectorPosition() const override;

protected:
    QVector3D position() const;

signals:

public slots:

};

#endif // POINT3D_CONTROL_H
