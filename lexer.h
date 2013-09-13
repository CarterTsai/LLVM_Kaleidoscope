#ifndef _LEXER_H_
#define _LEXER_H_

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <iostream>

enum Token {
    tok_eof = -1,
    tok_def = -2, tok_extern = -3,
    tok_identifier = -4, tok_number = -5,
};

extern std::string IdentifierStr;   // Filled in if tok_identifier
extern double NumVal;               // Filled in if tok_number
extern int CurTok;
extern std::map<char, int> BinopPrecedence;
// Function
int gettok();
int getNextToken();

#endif
