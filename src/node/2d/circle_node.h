#ifndef CIRCLE_NODE_H
#define CIRCLE_NODE_H

#include <Python.h>

#include <QObject>
#include "node/node.h"

class CircleNode : public Node
{
    Q_OBJECT
public:
    explicit CircleNode(QObject* parent=0);
    explicit CircleNode(QString name, QObject* parent=0);
    explicit CircleNode(QString name, QString x, QString y, QString r,
                        QObject* parent=0);
    explicit CircleNode(float x, float y, float z, float scale,
                        QObject* parent=0);

    NodeType::NodeType getNodeType() const override
        { return NodeType::CIRCLE; }
};

#endif // CIRCLE_NODE_H
