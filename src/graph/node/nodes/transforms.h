#ifndef NODE_TRANSFORMS_H
#define NODE_TRANSFORMS_H

class Node;
class NodeRoot;

Node* RotateXNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* RotateYNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* RotateZNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* ReflectXNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ReflectYNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ReflectZNode(float x, float y, float z, float scale, NodeRoot* parent);

Node* RecenterNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* TranslateNode(float x, float y, float z, float scale, NodeRoot* parent);

#endif
