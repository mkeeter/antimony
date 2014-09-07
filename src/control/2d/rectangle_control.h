#ifndef RECTANGLE_CONTROL_H
#define RECTANGLE_CONTROL_H

#include "control/wireframe.h"

class Point2DControl;
class Canvas;
class Node;

class RectangleControl : public WireframeControl
{
public:
    explicit RectangleControl(Canvas* canvas, Node* node);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    Point2DControl* a;
    Point2DControl* b;
};

#endif
