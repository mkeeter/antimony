#ifndef SCALEX_CONTROL_H
#define SCALEX_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class ScaleXHandle : public WireframeControl
{
public:
    explicit ScaleXHandle(Canvas* canvas, Node* node, bool pos,
                          QGraphicsItem* parent);
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    bool positive;
};

class ScaleXControl : public WireframeControl
{
public:
    explicit ScaleXControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    QVector3D position() const;

    ScaleXHandle* positive_handle;
    ScaleXHandle* negative_handle;
};

#endif
