#ifndef DUMMY_CONTROL_H
#define DUMMY_CONTROL_H

#include "control/control.h"

/*
 *  A DummyControl is non-interactive by default and below all other controls.
 */
class DummyControl : public Control
{
public:
    explicit DummyControl(Node* node, QObject* parent=0);
    void drag(QVector3D center, QVector3D delta) override;
    void paint(QMatrix4x4 m, QMatrix4x4 t, bool highlight, QPainter *painter);
    QRectF bounds(QMatrix4x4 m, QMatrix4x4 t) const override;
    QPainterPath shape(QMatrix4x4 m, QMatrix4x4 t) const override;
};

#endif
