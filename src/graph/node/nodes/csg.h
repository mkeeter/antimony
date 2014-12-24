#ifndef NODE_CSG_H
#define NODE_CSG_H

class Node;
class NodeRoot;

Node* DifferenceNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* IntersectionNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* UnionNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* BlendNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* OffsetNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ClearanceNode(float x, float y, float z, float scale, NodeRoot* parent);
Node* ShellNode(float x, float y, float z, float scale, NodeRoot* parent);

#endif

