#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "lexer.h"
#include "parser.h"

extern int CurTok;
// top ::= definition | external | expression | ';'
static void MainLoop() {
    while (1) {
        fprintf(stderr, "ready> ");
        switch(CurTok){
            case tok_eof: return;
            case ';': getNextToken(); break; // ignore top-level semicolons
            case tok_def: HandleDefinition(); break;
            case tok_extern: HandleExtern(); break;
            default: HandleTopLevelExpression(); break;
        }
    }
}

int main(int argc, const char *argv[])
{
    // Standard binary operator
    // 1 is lowest precedence
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;  // highest.  
   
    // Prime the first token 
    fprintf(stderr, "ready> ");
    getNextToken();

    // Run the main "interpreter Loop" now
    MainLoop();
    return 0;
}
