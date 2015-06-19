#include "fab/tree/tree.h"
#include "fab/tree/parser.h"
#include "fab/tree/node/node.h"
#include "fab/tree/node/opcodes.h"

#include <stack>

typedef struct env_
{
	bool valid;
	Node* head;
	Node* Xnode;
	Node* Ynode;
	Node* Znode;
	NodeCache* cache;
	Node *tempX, *tempY, *tempZ;
	std::stack <Node*>	*nodestack;
} Env;
