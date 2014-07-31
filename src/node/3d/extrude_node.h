#ifndef EXTRUDE_NODE_H
#define EXTRUDE_NODE_H

#include "node/node.h"

class ExtrudeNode : public Node
{
public:
    explicit ExtrudeNode(QObject* parent=0);
    explicit ExtrudeNode(float x, float y, float z, float scale,
                          QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::EXTRUDE; }
};

#endif
