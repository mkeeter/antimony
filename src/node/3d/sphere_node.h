#ifndef SPHERE_NODE_H
#define SPHERE_NODE_H

#include "node/node.h"

class SphereNode : public Node
{
public:
    explicit SphereNode(QObject* parent=0);
    explicit SphereNode(float x, float y, float z, float scale,
                        QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::SPHERE; }
};

#endif // CIRCLE_NODE_H
