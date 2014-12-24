#ifndef RECENTER_CONTROL_H
#define RECENTER_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class RecenterControl : public WireframeControl
{
public:
    explicit RecenterControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

#endif
