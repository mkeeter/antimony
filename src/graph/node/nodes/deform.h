#ifndef NODE_DEFORM_H
#define NODE_DEFORM_H

#include <QString>

class Node;
class NodeRoot;

Node* AttractNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* RepelNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ScaleXNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ScaleYNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ScaleZNode(float x, float y, float z, float scale, NodeRoot* parent);

#endif
