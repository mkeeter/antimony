#ifndef ROTATEX_CONTROL_H
#define ROTATEX_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class RotateXHandle : public WireframeControl
{
public:
    explicit RotateXHandle(Canvas* canvas, Node* node,
                           QGraphicsItem* parent);
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QPair<QVector3D, float>> points() const override;
protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::green; }
};

////////////////////////////////////////////////////////////////////////////////

class RotateXControl : public WireframeControl
{
public:
    explicit RotateXControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::green; }

    RotateXHandle* handle;
};

#endif
