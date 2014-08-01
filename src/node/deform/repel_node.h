#ifndef REPEL_NODE
#define REPEL_NODE

#include "node/node.h"

class RepelNode : public Node
{
public:
    explicit RepelNode(QObject* parent=0);
    explicit RepelNode(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::REPEL; }
};

#endif
