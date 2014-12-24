#ifndef NODE_META_H
#define NODE_META_H

#include <QString>

class Node;
class NodeRoot;

Node* ScriptNode(QString name, QString x, QString y, QString z,
                 QString script, NodeRoot* parent);
Node* ScriptNode(float x, float y, float z, float scale,
                 NodeRoot* parent);

#endif

