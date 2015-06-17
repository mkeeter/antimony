#ifndef NODE_CONSTRUCTOR_H
#define NODE_CONSTRUCTOR_H

#include <functional>

class Node;
class NodeRoot;

typedef Node* (*NodeConstructor)(NodeRoot*);
typedef std::function<Node* (NodeRoot*)> NodeConstructorFunction;

#endif
