#ifndef SCALEY_CONTROL_H
#define SCALEY_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class _ScaleYHandle : public WireframeControl
{
public:
    explicit _ScaleYHandle(Node* node, bool pos, QObject* parent);
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    bool positive;
};

class ScaleYControl : public WireframeControl
{
public:
    explicit ScaleYControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    QVector3D position() const;

    _ScaleYHandle* positive_handle;
    _ScaleYHandle* negative_handle;
};

#endif
