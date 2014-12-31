#ifndef NODE_CONSTRUCTOR_H
#define NODE_CONSTRUCTOR_H

class Node;
class NodeRoot;

typedef Node* (*NodeConstructor)(float, float, float, float, NodeRoot*);

#endif
