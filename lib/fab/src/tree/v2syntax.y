%include {
	#include <math.h>
	#include <cassert>
	#include <iostream>
	#include "fab/tree/v2parser.hpp"

	extern "C"
	{
		Node* get_cached_node(NodeCache* const cache, Node* const n);
	}

	#define CACHED(n) get_cached_node(environment->cache, n)
	// #define CACHED(n) n
}

%name v2Parse
%extra_argument {Env* environment}

%token_type {const char*}
%token_prefix TOKEN_

%type v1_expr {Node* }
%type v1_assignment_expr {Node* }
%type expr {Node* }
%type assignment_expr {Node* }

%left PLUS MINUS.
%left MUL DIV.
%right DOUBLESTAR.
%right UMINUS.

%syntax_error
{
	environment->valid = false;
	int n = sizeof(yyTokenName) / sizeof(yyTokenName[0]);
	for (int i = 0; i < n; ++i) {
		int a = yy_find_shift_action(yypParser, (YYCODETYPE)i);
		if (a < YYNSTATE + YYNRULE) {
		printf("possible token: %s\n", yyTokenName[i]);
		}
	}
}


program 	::= v1_expr(E).						{	environment->head = E; 	}

v1_assignment_expr(E) 	::= v1_expr(O).			{	E = O;   			}
v1_assignment_expr(E) 	::= V1SKIP.    			{	E = NULL;			}

v1_assignment_exprs 		::=  v1_assignment_expr(I) v1_assignment_expr(J) v1_assignment_expr(K).
			{
				environment->nodestack->push(environment->Xnode);
				environment->nodestack->push(environment->Ynode);
				environment->nodestack->push(environment->Znode);

				environment->tempX = I ? CACHED(I) : environment->Xnode;
				environment->tempY = J ? CACHED(J) : environment->Ynode;
				environment->tempZ = K ? CACHED(K) : environment->Znode;

				environment->Xnode = environment->tempX;
				environment->Ynode = environment->tempY;
				environment->Znode = environment->tempZ;
			}

v1_expr(E) 			::= V1MAP v1_assignment_exprs v1_expr(O).
			{
				E = O;
				environment->Znode = environment->nodestack->top();
				environment->nodestack->pop();
				environment->Ynode = environment->nodestack->top();
				environment->nodestack->pop();
				environment->Xnode = environment->nodestack->top();
				environment->nodestack->pop();
			}

v1_expr(E)	::= V1PLUS v1_expr(L) v1_expr(R). 			{	E = CACHED(add_n(L, R)); 	}
v1_expr(E)	::= V1MINUS v1_expr(L) v1_expr(R).			{	E = CACHED(sub_n(L, R)); 	}
v1_expr(E)	::= V1MUL v1_expr(L) v1_expr(R).  			{	E = CACHED(mul_n(L, R)); 	}
v1_expr(E)	::= V1DIV v1_expr(L) v1_expr(R).  			{	E = CACHED(div_n(L, R)); 	}
v1_expr(E)	::= V1MIN v1_expr(L) v1_expr(R).  			{	E = CACHED(min_n(L, R)); 	}
v1_expr(E)	::= V1MAX v1_expr(L) v1_expr(R).  			{	E = CACHED(max_n(L, R)); 	}
v1_expr(E)	::= V1POW v1_expr(L) v1_expr(R).  			{	E = CACHED(pow_n(L, R)); 	}

v1_expr(E)	::= V1SIN v1_expr(O).   				{	E = CACHED(sin_n(O)); 	}
v1_expr(E)	::= V1COS v1_expr(O).   				{	E = CACHED(cos_n(O)); 	}
v1_expr(E)	::= V1TAN v1_expr(O).   				{	E = CACHED(tan_n(O)); 	}
v1_expr(E)	::= V1ASIN v1_expr(O).  				{	E = CACHED(asin_n(O)); 	}
v1_expr(E)	::= V1ACOS v1_expr(O).  				{	E = CACHED(acos_n(O)); 	}
v1_expr(E)	::= V1ATAN v1_expr(O).  				{	E = CACHED(atan_n(O)); 	}
v1_expr(E)	::= V1ABS v1_expr(O).   				{	E = CACHED(abs_n(O)); 	}
v1_expr(E)	::= V1SQUARE v1_expr(O).				{	E = CACHED(square_n(O)); 	}
v1_expr(E)	::= V1SQRT v1_expr(O).  				{	E = CACHED(sqrt_n(O)); 	}
v1_expr(E)	::= V1NEG v1_expr(O).   				{	E = CACHED(neg_n(O)); 	}
v1_expr(E)	::= V1EXP v1_expr(O).   				{	E = CACHED(exp_n(O)); 	}
v1_expr(E)	::= CONSTANT V1MINUS V1FLOAT(F).		{	E = CACHED(constant_n(-atof(F))); 	}
v1_expr(E)	::= CONSTANT V1FLOAT(F).				{	E = CACHED(constant_n(atof(F))); 	}

v1_expr(E)	::= V1X.						{	E = CACHED(environment->Xnode); 	}
v1_expr(E)	::= V1Y.						{	E = CACHED(environment->Ynode); 	}
v1_expr(E)	::= V1Z.						{	E = CACHED(environment->Znode); 	}

v1_expr(E)	::= EQUAL expr(V) SEMICOLON.			{	E = V; }



expr(E) 	::= LBRACKET v1_expr(V) RBRACKET.		{	E = V; }
expr(E) 	::= LPAREN expr(O) RPAREN.       		 	{	E = O; }




expr(E) 	::= FLOAT(F).						{	E = CACHED(constant_n(atof(F))); 	}
expr(E) 	::= X.       						{	E = CACHED(environment->Xnode); 	}
expr(E) 	::= Y.       						{	E = CACHED(environment->Ynode); 	}
expr(E) 	::= Z.       						{	E = CACHED(environment->Znode); 	}

expr(E)		::= expr(L) PLUS expr(R).				{	E = CACHED(add_n(L, R)); 	}
expr(E)		::= expr(L) MINUS expr(R).				{	E = CACHED(sub_n(L, R)); 	}
expr(E)		::= expr(L) MUL expr(R).				{	E = CACHED(mul_n(L, R)); 	}
expr(E)		::= expr(L) DIV expr(R).				{	E = CACHED(div_n(L, R)); 	}
expr(E)		::= expr(L) DOUBLESTAR expr(R).			{	E = CACHED(pow_n(L, R)); 	}

expr(E)		::= MIN LPAREN expr(L) COMMA expr(R) RPAREN.		{	E = CACHED(min_n(L, R)); 	}
expr(E)		::= MAX LPAREN expr(L) COMMA expr(R) RPAREN.	{	E = CACHED(max_n(L, R)); 	}
expr(E)		::= POW LPAREN expr(L) COMMA expr(R) RPAREN.	{	E = CACHED(pow_n(L, R)); 	}

expr(E)		::= SIN LPAREN expr(O) RPAREN.			{	E = CACHED(sin_n(O)); 	}
expr(E)		::= COS LPAREN expr(O) RPAREN.			{	E = CACHED(cos_n(O)); 	}
expr(E)		::= TAN LPAREN expr(O) RPAREN.			{	E = CACHED(tan_n(O)); 	}
expr(E)		::= ASIN LPAREN expr(O) RPAREN.			{	E = CACHED(asin_n(O)); 	}
expr(E)		::= ACOS LPAREN expr(O) RPAREN.		{	E = CACHED(acos_n(O)); 	}
expr(E)		::= ATAN LPAREN expr(O) RPAREN.		{	E = CACHED(atan_n(O)); 	}
expr(E)		::= ABS LPAREN expr(O) RPAREN.			{	E = CACHED(abs_n(O)); 	}
expr(E)		::= SQRT LPAREN expr(O) RPAREN.		{	E = CACHED(sqrt_n(O)); 	}
expr(E)		::= MINUS expr(O). [UMINUS]			{	E = CACHED(neg_n(O)); 	}
expr(E)		::= EXP LPAREN expr(O) RPAREN.			{	E = CACHED(exp_n(O)); 	}

expr(E)		::= MAP assignment_exprs LBRACE expr(O) RBRACE.
			{
				E = O;
				environment->Znode = environment->nodestack->top();
				environment->nodestack->pop();
				environment->Ynode = environment->nodestack->top();
				environment->nodestack->pop();
				environment->Xnode = environment->nodestack->top();
				environment->nodestack->pop();
			}

assignment_expr(E) 	::= expr(O).					{ 	E = O; 		}
assignment_expr(E) 	::= SKIP.  					{ 	E = NULL; 	}
assignment_exprs ::=  LPAREN assignment_expr(I) COMMA assignment_expr(J) COMMA assignment_expr(K) RPAREN.
			{
				environment->nodestack->push(environment->Xnode);
				environment->nodestack->push(environment->Ynode);
				environment->nodestack->push(environment->Znode);

				environment->tempX = I ? CACHED(I) : environment->Xnode;
				environment->tempY = J ? CACHED(J) : environment->Ynode;
				environment->tempZ = K ? CACHED(K) : environment->Znode;

				environment->Xnode = environment->tempX;
				environment->Ynode = environment->tempY;
				environment->Znode = environment->tempZ;
			}
