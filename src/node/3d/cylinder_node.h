#ifndef CYLINDER_NODE_H
#define CYLINDER_NODE_H

#include "node/node.h"

class CylinderNode : public Node
{
public:
    explicit CylinderNode(QObject* parent=0);
    explicit CylinderNode(float x, float y, float z, float scale,
                          QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::CYLINDER; }
};

#endif
