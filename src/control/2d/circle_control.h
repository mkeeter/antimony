#ifndef CIRCLE_CONTROL_H
#define CIRCLE_CONTROL_H

#include <Python.h>

#include <QObject>
#include <QVector3D>

#include "control/control.h"
#include "control/multiline.h"
#include "control/2d/point2d_control.h"

class _RadiusControl : public MultiLineControl
{
    Q_OBJECT
public:
    explicit _RadiusControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    virtual QVector<QVector<QVector3D>> lines() const override;
    virtual void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

////////////////////////////////////////////////////////////////////////////////

class CircleControl : public DummyControl
{
    Q_OBJECT
public:
    explicit CircleControl(Canvas* canvas, Node* node);

    /** Drags the circle in x and y.
     */
    virtual void drag(QVector3D center, QVector3D delta) override;

    /** Returns a position for the inspector.
     */
    virtual QPointF inspectorPosition() const override;

    virtual QRectF boundingRect() const override;

protected:
    QVector3D position() const;

    _RadiusControl* radius;
    Point2DControl* center;
};

#endif // CIRCLE_CONTROL_H
