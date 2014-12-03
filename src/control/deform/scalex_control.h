#ifndef SCALEX_CONTROL_H
#define SCALEX_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class _ScaleXHandle : public WireframeControl
{
public:
    explicit _ScaleXHandle(Node* node, bool pos, QObject* parent);
    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    bool positive;
};

class ScaleXControl : public WireframeControl
{
public:
    explicit ScaleXControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
protected:
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;
    QColor defaultPenColor() const override
        { return Colors::blue; }
    QVector3D position() const;

    _ScaleXHandle* positive_handle;
    _ScaleXHandle* negative_handle;
};

#endif
