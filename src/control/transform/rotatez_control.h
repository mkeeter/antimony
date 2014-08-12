#ifndef ROTATEZ_CONTROL_H
#define ROTATEZ_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class RotateZHandle : public WireframeControl
{
public:
    explicit RotateZHandle(Canvas* canvas, Node* node,
                           QGraphicsItem* parent);
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QPair<QVector3D, float>> points() const override;
protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

////////////////////////////////////////////////////////////////////////////////

class RotateZControl : public WireframeControl
{
public:
    explicit RotateZControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }

    RotateZHandle* handle;
};

#endif
