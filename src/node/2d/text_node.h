#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "node/node.h"

class TextNode : public Node
{
    Q_OBJECT
public:
    explicit TextNode(QObject* parent=0);
    explicit TextNode(float x, float y, float z, float scale,
                      QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::TEXT; }
};

#endif
