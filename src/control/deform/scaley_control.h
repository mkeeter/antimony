#ifndef SCALEY_CONTROL_H
#define SCALEY_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class ScaleYHandle : public WireframeControl
{
public:
    explicit ScaleYHandle(Canvas* canvas, Node* node, bool pos,
                          QGraphicsItem* parent);
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QColor defaultPenColor() const override
        { return Colors::orange; }
    bool positive;
};

class ScaleYControl : public WireframeControl
{
public:
    explicit ScaleYControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;
    QColor defaultPenColor() const override
        { return Colors::orange; }
    QVector3D position() const;

    ScaleYHandle* positive_handle;
    ScaleYHandle* negative_handle;
};

#endif
