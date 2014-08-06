#ifndef SLIDER_CONTROL_H
#define SLIDER_CONTROL_H

#include "control/control.h"

class SliderHandle : public Control
{
public:
    explicit SliderHandle(Canvas* canvas, Node* node,
                          QGraphicsItem* parent=0);

    QRectF bounds() const override;

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QPointF position() const;
    void paintControl(QPainter* painter) override;
};

////////////////////////////////////////////////////////////////////////////////

class SliderControl : public Control
{
public:
    explicit SliderControl(Canvas* canvas, Node* node);

    QRectF bounds() const override;

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QPointF position() const;
    void paintControl(QPainter* painter) override;
    QPointF inspectorPosition() const override;

    SliderHandle* handle;
};

#endif
