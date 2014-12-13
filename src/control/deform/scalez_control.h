#ifndef SCALEZ_CONTROL_H
#define SCALEZ_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class _ScaleZHandle : public WireframeControl
{
public:
    explicit _ScaleZHandle(Node* node, bool pos, QObject* parent);
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    bool positive;
};

class ScaleZControl : public WireframeControl
{
public:
    explicit ScaleZControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    QVector3D position() const;

    _ScaleZHandle* positive_handle;
    _ScaleZHandle* negative_handle;
};

#endif
