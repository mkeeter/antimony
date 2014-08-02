#ifndef ROTATEX_NODE
#define ROTATEX_NODE

#include "node/node.h"

class RotateXNode : public Node
{
public:
    explicit RotateXNode(QObject* parent=0);
    explicit RotateXNode(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::ROTATEX; }
};

#endif
