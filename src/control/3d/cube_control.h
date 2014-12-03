#ifndef CUBE_CONTROL_H
#define CUBE_CONTROL_H

#include "control/wireframe.h"

class Point3DControl;
class Node;

class CubeControl : public WireframeControl
{
public:
    explicit CubeControl(Node* node, QObject* parent=NULL);
    QVector<QVector<QVector3D>> lines() const override;
    void drag(QVector3D center, QVector3D delta) override;
protected:
    Point3DControl* a;
    Point3DControl* b;
};

#endif // CUBE_CONTROL_H
