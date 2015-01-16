#ifndef ITERATE_POLAR_CONTROL_H
#define ITERATE_POLAR_CONTROL_H

#include <QVector3D>

#include "control/wireframe.h"
#include "ui/util/colors.h"

class IteratePolarControl : public WireframeControl
{
public:
    explicit IteratePolarControl(Node* node, QObject* parent=0);

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines(QMatrix4x4 m, QMatrix4x4 t) const override;
    QVector<QPair<QVector3D, float>> points() const override;

    QVector3D position() const;

    QColor defaultPenColor() const
        { return Colors::teal; }
};

#endif
