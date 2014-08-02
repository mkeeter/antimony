#ifndef TEXT_CONTROL_H
#define TEXT_CONTROL_H

#include "control/wireframe.h"

class _CaretTopControl : public WireframeControl
{
public:
    explicit _CaretTopControl(Canvas* canvas, Node* node, QGraphicsItem* parent);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QVector3D center() const;
};

class TextControl : public WireframeControl
{
public:
    explicit TextControl(Canvas* canvas, Node* node);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    _CaretTopControl* caret_top;
};

#endif
