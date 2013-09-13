#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "lexer.h"

extern int CurTok; 

// Return the next token from Standard input
int gettok(){
    static int LastChar = ' ';
    // Skip any whitespace
    while (isspace(LastChar)) {
        LastChar = getchar();
    }  
    
    // Identifier: [a-zA-Z][a-zA-Z0-9]    
    if (isalpha(LastChar)) { // check [a-zA-Z]
        IdentifierStr = LastChar;
        while (isalnum( ( LastChar = getchar() ) ) ) { //check [a-zA-Z0-9]
            IdentifierStr += LastChar;
        }

        if (IdentifierStr == "def") return tok_def;
        if (IdentifierStr == "extern") return tok_extern;
        
        return tok_identifier;
    }    

    // Number: [0-9.]+
    if (isdigit(LastChar) || LastChar == '.') { 
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');
        
        NumVal = strtod(NumStr.c_str(), 0);
        return tok_number;
    }
    
    if (LastChar == '#') {
        do  LastChar = getchar();
        while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');
        
        if (LastChar != EOF) {
            return gettok();
        }
    }
    
    if( LastChar == EOF )
        return tok_eof;
    // Otherwise, just return the character as its ascii value
    int ThisChar = LastChar;
    LastChar = getchar();
    return ThisChar;
}

// CurTok/getNextToken - Provide a simple token buffer. CurTok is the current
// token the parser is looking at. getNextToken reads another token from the 
// lexer and updates CurTok with its results.

int getNextToken() {
    return CurTok = gettok();
}

