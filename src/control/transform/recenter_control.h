#ifndef RECENTER_CONTROL_H
#define RECENTER_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class RecenterControl : public WireframeControl
{
public:
    explicit RecenterControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::green; }
};

#endif
