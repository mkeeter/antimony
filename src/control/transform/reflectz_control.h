#ifndef REFLECTZ_CONTROL_H
#define REFLECTZ_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class ReflectZControl : public WireframeControl
{
public:
    explicit ReflectZControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QVector<QVector<QVector3D>> lines() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

#endif

