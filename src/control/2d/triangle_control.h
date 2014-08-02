#ifndef TRIANGLE_CONTROL_H
#define TRIANGLE_CONTROL_H

#include "control/multiline.h"

class Point2DControl;
class Canvas;
class Node;

class TriangleControl : public MultiLineControl
{
    Q_OBJECT
public:
    explicit TriangleControl(Canvas* canvas, Node* node);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
protected:
    Point2DControl* a;
    Point2DControl* b;
    Point2DControl* c;
};

#endif // TRIANGLE_CONTROL_H
