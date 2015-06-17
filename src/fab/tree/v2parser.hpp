#include "tree/tree.h"
#include "tree/parser.h"
#include "tree/node/node.h"
#include "tree/node/opcodes.h"
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
