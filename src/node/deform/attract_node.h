#ifndef ATTRACT_NODE
#define ATTRACT_NODE

#include "node/node.h"

class AttractNode : public Node
{
public:
    explicit AttractNode(QObject* parent=0);
    explicit AttractNode(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::ATTRACT; }
};

#endif
