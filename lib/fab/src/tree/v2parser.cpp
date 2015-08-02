#include <cstdlib>
#include <iostream>
#include <string>
#include <stdbool.h>

#include "fab/tree/v2parser.hpp"
#include "fab/tree/tree.h"
#include "fab/tree/parser.h"
#include "fab/tree/node/node.h"
#include "fab/tree/node/opcodes.h"
#include "fab/tree/node/printers.h"

// Generated files live in the build directory
#include "v2syntax.lemon.hpp"
#include "v2syntax.yy.hpp"

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_RESET    "\x1b[0m"

using namespace std;

extern "C"
{
    bool v2parse(Node **result, const char* input, Node* X, Node* Y, Node *Z, NodeCache* cache);
}

void* v2ParseAlloc(void* (*allocProc)(size_t));
void v2Parse(void* parser, int token, const char*, Env* environment);
void v2ParseFree(void* parser, void(*freeProc)(void*));
void v2ParseTrace(FILE*, char*);

bool v2parse(Node **result, const char* input, Node* X, Node* Y, Node *Z, NodeCache* cache)
{
    #ifdef PARSEDEBUG
        printf(ANSI_COLOR_BLUE "Parsing input:" ANSI_COLOR_RESET " \t'%s'\n");
        v2ParseTrace(stdout, "\t" ANSI_COLOR_YELLOW "\tParse trace:\t" ANSI_COLOR_RESET);
    #endif

    Env* locals = (Env*) malloc(sizeof(Env));
    locals->valid = true;
    locals->head = NULL;
    locals->Xnode = X;
    locals->Ynode = Y;
    locals->Znode = Z;
    locals->cache = cache;
    locals->nodestack = new std::stack<Node*>();


    yyscan_t scanner;
    yylex_init(&scanner);
    YY_BUFFER_STATE bufferState = yy_scan_string(input, scanner);
    void *parser = v2ParseAlloc(malloc);

    char *text;
    int lexCode;

    do {
        lexCode = yylex(scanner);
        text = yyget_text(scanner);
        v2Parse(parser, lexCode, text, locals);
    } while (lexCode > 0 &&  locals->valid);


    if (-1 == lexCode || !locals->valid) {
        printf(ANSI_COLOR_RED "Parse failure on input of:" ANSI_COLOR_RESET " '%s'\n", input);
        return false;
    }

    *result = locals->head;

    #ifdef PARSEDEBUG
        printf(ANSI_COLOR_GREEN "Parse success:\t" ANSI_COLOR_RESET);
        print_node(*result);
        printf("\n");
    #endif

    yy_delete_buffer(bufferState, scanner);
    yylex_destroy(scanner);
    v2ParseFree(parser, free);
    free(locals->nodestack);
    free(locals);

    return true;
}
