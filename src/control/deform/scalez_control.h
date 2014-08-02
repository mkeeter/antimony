#ifndef SCALEZ_CONTROL_H
#define SCALEZ_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class ScaleZHandle : public WireframeControl
{
public:
    explicit ScaleZHandle(Canvas* canvas, Node* node, bool pos,
                          QGraphicsItem* parent);
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QColor defaultPenColor() const override
        { return Colors::orange; }
    bool positive;
};

class ScaleZControl : public WireframeControl
{
public:
    explicit ScaleZControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;
    QColor defaultPenColor() const override
        { return Colors::orange; }
    QVector3D position() const;

    ScaleZHandle* positive_handle;
    ScaleZHandle* negative_handle;
};

#endif
