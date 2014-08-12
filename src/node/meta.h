#ifndef NODE_META_H
#define NODE_META_H

#include <QObject>
#include <QString>

class Node;

Node* ScriptNode(QString name, QString x, QString y, QString z,
                 QString script, QObject* parent=0);
Node* ScriptNode(float x, float y, float z, float scale,
                 QObject* parent=0);

#endif

