#ifndef SCALEX_NODE
#define SCALEX_NODE

#include "node/node.h"

class ScaleXNode : public Node
{
public:
    explicit ScaleXNode(QObject* parent=0);
    explicit ScaleXNode(float x, float y, float z, float scale,
                     QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::SCALEX; }
};

#endif
