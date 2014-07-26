#ifndef CUBE_NODE_H
#define CUBE_NODE_H

#include <Python.h>

#include "node/node.h"

class Point3D;

class CubeNode : public Node
{
    Q_OBJECT
public:
    explicit CubeNode(QObject* parent=0);
    explicit CubeNode(QString name, QObject* parent=0);
    explicit CubeNode(float x, float y, float z, float scale,
                      QObject* parent = NULL);

    NodeType::NodeType getNodeType() const override
        { return NodeType::CUBE; }

protected:
    Point3D* a;
    Point3D* b;
};

#endif // CUBE_NODE_H
