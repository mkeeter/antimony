#ifndef NODE_TRANSFORMS_H
#define NODE_TRANSFORMS_H

#include <QObject>
#include <QString>

class Node;

Node* RotateXNode(float x, float y, float z, float scale, QObject* parent=0);

#endif
