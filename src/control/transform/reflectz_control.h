#ifndef REFLECTZ_CONTROL_H
#define REFLECTZ_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class ReflectZControl : public WireframeControl
{
public:
    explicit ReflectZControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

#endif

