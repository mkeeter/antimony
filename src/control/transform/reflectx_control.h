#ifndef REFLECTX_CONTROL_H
#define REFLECTX_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class ReflectXControl : public WireframeControl
{
public:
    explicit ReflectXControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

#endif
