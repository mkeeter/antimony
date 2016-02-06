#ifndef NODE_CONSTRUCTOR_H
#define NODE_CONSTRUCTOR_H

#include <functional>

class Node;
class Graph;

typedef Node* (*NodeConstructor)(Graph*);
typedef std::function<Node* (Graph*)> NodeConstructorFunction;

#endif
