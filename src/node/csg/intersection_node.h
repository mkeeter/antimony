#ifndef INTERSECTION_NODE
#define INTERSECTION_NODE

#include "node/node.h"

class IntersectionNode : public Node
{
public:
    explicit IntersectionNode(QObject* parent=0);
    explicit IntersectionNode(float x, float y, float z, float scale,
                              QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::INTERSECTION; }
};

#endif
