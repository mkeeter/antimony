#ifndef EXTRUDE_CONTROL_H
#define EXTRUDE_CONTROL_H

#include "control/control.h"
#include "control/wireframe.h"

class _ExtrudeSpanControl : public WireframeControl
{
public:
    explicit _ExtrudeSpanControl(Node* node, QObject* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QColor defaultPenColor() const override;
};

class ExtrudeControl : public WireframeControl
{
public:
    explicit ExtrudeControl(Node* node, QObject* parent=NULL);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QColor defaultPenColor() const override;
    _ExtrudeSpanControl* span;
};

#endif
