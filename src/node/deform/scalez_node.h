#ifndef SCALEZ_NODE
#define SCALEZ_NODE

#include "node/node.h"

class ScaleZNode : public Node
{
public:
    explicit ScaleZNode(QObject* parent=0);
    explicit ScaleZNode(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::SCALEZ; }
};

#endif
