#ifndef NODE_CSG_H
#define NODE_CSG_H

#include <QObject>
#include <QString>

class Node;

Node* DifferenceNode(float x, float y, float z, float scale, QObject* parent=0);
Node* IntersectionNode(float x, float y, float z, float scale, QObject* parent=0);
Node* UnionNode(float x, float y, float z, float scale, QObject* parent=0);
Node* OffsetNode(float x, float y, float z, float scale, QObject* parent=0);
Node* ClearanceNode(float x, float y, float z, float scale, QObject* parent=0);

#endif

