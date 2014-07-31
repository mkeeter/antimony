#ifndef DIFFERENCE_NODE
#define DIFFERENCE_NODE

#include "node/node.h"

class DifferenceNode : public Node
{
public:
    explicit DifferenceNode(QObject* parent=0);
    explicit DifferenceNode(float x, float y, float z, float scale,
                            QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::DIFFERENCE; }
};

#endif
