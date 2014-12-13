#ifndef ROTATEZ_CONTROL_H
#define ROTATEZ_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class _RotateZHandle : public WireframeControl
{
public:
    explicit _RotateZHandle(Node* node, QObject* parent);
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QPair<QVector3D, float>> points() const override;
protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

////////////////////////////////////////////////////////////////////////////////

class RotateZControl : public WireframeControl
{
public:
    explicit RotateZControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }

    _RotateZHandle* handle;
};

#endif
