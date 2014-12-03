#ifndef ROTATEY_CONTROL_H
#define ROTATEY_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class _RotateYHandle : public WireframeControl
{
public:
    explicit _RotateYHandle(Node* node, QObject* parent);
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QPair<QVector3D, float>> points() const override;
protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }
};

////////////////////////////////////////////////////////////////////////////////

class RotateYControl : public WireframeControl
{
public:
    explicit RotateYControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }

    _RotateYHandle* handle;
};

#endif
