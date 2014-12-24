#ifndef RECTANGLE_CONTROL_H
#define RECTANGLE_CONTROL_H

#include "control/wireframe.h"

class Point2DControl;
class Node;

class RectangleControl : public WireframeControl
{
public:
    explicit RectangleControl(Node* node, QObject* parent=NULL);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
protected:
    Point2DControl* a;
    Point2DControl* b;
};

#endif
