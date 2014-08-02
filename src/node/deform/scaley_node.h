#ifndef SCALEY_NODE
#define SCALEY_NODE

#include "node/node.h"

class ScaleYNode : public Node
{
public:
    explicit ScaleYNode(QObject* parent=0);
    explicit ScaleYNode(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::SCALEY; }
};

#endif
