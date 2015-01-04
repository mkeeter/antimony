#ifndef ITERATE_H
#define ITERATE_H

class Node;
class NodeRoot;

Node* Iterate2DNode(float x, float y, float z, float scale,
                    NodeRoot* parent);
Node* IteratePolarNode(float x, float y, float z, float scale,
                       NodeRoot* parent);

#endif
