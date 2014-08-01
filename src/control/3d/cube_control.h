#ifndef CUBE_CONTROL_H
#define CUBE_CONTROL_H

#include "control/multiline.h"

class Point3DControl;
class Canvas;
class Node;

class CubeControl : public MultiLineControl
{
public:
    explicit CubeControl(Canvas* canvas, Node* node);
    virtual QVector<QVector<QVector3D>> lines() const override;
    virtual void drag(QVector3D center, QVector3D delta);
    virtual QPointF inspectorPosition() const override;
protected:
    Point3DControl* a;
    Point3DControl* b;
};

#endif // CUBE_CONTROL_H
