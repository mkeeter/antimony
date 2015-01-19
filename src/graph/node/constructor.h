#ifndef NODE_CONSTRUCTOR_H
#define NODE_CONSTRUCTOR_H

#include <functional>

class Node;
class NodeRoot;

typedef Node* (*NodeConstructor)(float, float, float, float, NodeRoot*);
typedef std::function<Node* (float, float, float, float, NodeRoot*)>
        NodeConstructorFunction;

#endif
