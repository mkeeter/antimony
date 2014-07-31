#ifndef UNION_NODE
#define UNION_NODE

#include "node/node.h"

class UnionNode : public Node
{
public:
    explicit UnionNode(QObject* parent=0);
    explicit UnionNode(float x, float y, float z, float scale,
                       QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::UNION; }
};

#endif
