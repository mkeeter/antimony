#ifndef SCRIPT_NODE_H
#define SCRIPT_NODE_H

#include <Python.h>
#include <QObject>
#include "node/node.h"

class ScriptNode : public _Node<NodeType::SCRIPT>
{
    Q_OBJECT
public:
    explicit ScriptNode(QString name, QString x, QString y, QString z,
                        QString script, QObject* parent=NULL);
    explicit ScriptNode(float x, float y, float z, float scale,
                        QObject* parent=NULL);
};

#endif // SCRIPT_NODE_H
