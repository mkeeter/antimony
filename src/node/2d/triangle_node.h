#ifndef TRIANGLE_NODE_H
#define TRIANGLE_NODE_H

#include "node/node.h"

class TriangleNode : public Node
{
public:
    explicit TriangleNode(QObject* parent=0);
    explicit TriangleNode(float x, float y, float z, float scale,
                          QObject* parent=0);
    NodeType::NodeType getNodeType() const override
        { return NodeType::TRIANGLE; }
};

#endif
