#ifndef SCRIPT_NODE_H
#define SCRIPT_NODE_H

#include <Python.h>
#include <QObject>
#include "node/node.h"

class ScriptNode : public Node
{
    Q_OBJECT
public:
    explicit ScriptNode(QString name, QString x, QString y, QString z,
                        QString script, QObject* parent=NULL);
    explicit ScriptNode(float x, float y, float z, float scale,
                        QObject* parent=NULL);

    NodeType::NodeType getNodeType() const override
        { return NodeType::SCRIPT; }
};

#endif // SCRIPT_NODE_H
