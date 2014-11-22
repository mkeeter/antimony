#ifndef NODE_DEFORM_H
#define NODE_DEFORM_H

#include <QObject>
#include <QString>

class Node;

Node* AttractNode(float x, float y, float z, float scale, QObject* parent=0);
Node* RepelNode(float x, float y, float z, float scale, QObject* parent=0);
Node* ScaleXNode(float x, float y, float z, float scale, QObject* parent=0);
Node* ScaleYNode(float x, float y, float z, float scale, QObject* parent=0);
Node* ScaleZNode(float x, float y, float z, float scale, QObject* parent=0);

#endif
