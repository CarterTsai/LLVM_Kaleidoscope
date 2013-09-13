#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "parser.h"
#include "error.h"

double NumVal;
int CurTok;
std::string IdentifierStr;
std::map<char, int> BinopPrecedence; 


static ExprAST *ParsePrimary() {
    switch (CurTok) {
        default: 
            printf("--> %c\n",CurTok);
            return Error("unknown token when expecting an expression");
        case tok_identifier: return ParseIdentifierExpr();
        case tok_number:     return ParseNumberExpr();
        case '(':            return ParseParenExpr();
    }
}

// expression
//   ::= primary binoprhs

static ExprAST *ParseExpression() {
    ExprAST *LHS = ParsePrimary();
    if (!LHS) return 0;

    return ParseBinOpRHS(0, LHS);
}

// numberexpre ::= number
static ExprAST *ParseNumberExpr() {
    ExprAST *Result = new NumberExprAST(NumVal);
    getNextToken();
    return Result;
}

// parenexpr ::= '(' expression ')'
static ExprAST *ParseParenExpr() {
    getNextToken(); // eat (
    ExprAST *V = ParseExpression();
    if (!V) return 0;

    if(CurTok != ')')
        return Error("expected ')'");
    getNextToken(); // eat )
    return V;
}

// Identifierexpr
//  ::= Identifier
//  ::= Identifier '(' expression* ')'
static ExprAST *ParseIdentifierExpr() {
    std::string IdName = IdentifierStr;
    getNextToken(); // eat identifier
    
    if ( CurTok != '(') // Simple variable ref
        return new VariableExprAST(IdName);
	
    // Call
    getNextToken();
    std::vector<ExprAST*> Args;
    if (CurTok != ')') {
        while(1) {
            ExprAST *Arg = ParseExpression();
            if (!Arg) return 0;
            Args.push_back(Arg);

            if (CurTok == ')') break;

            if (CurTok != ',')
                return Error("Expectd ')' or ',' in argument list");
            getNextToken();
        }
    }

    // Eat the ')'
    getNextToken();

    return new CallExprAST(IdName, Args);
}

// GetTokPrecedence - Get the precedence of the pending binary operator token 
static int GetTokPrecedence() {
    if (!isascii(CurTok))
        return -1;
    // Make sure it's a declared binop
    int TokPrec = BinopPrecedence[CurTok];
    if(TokPrec <= 0) return -1;
    return TokPrec;
} 

// binoprhs
// ::= ('+' primary)*
static ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
    // if this is a binop, find its precedence
    while (1) {
        int TokPrec = GetTokPrecedence();
        // if this is a binop that binds at least as tightly as the current
        // binop,  consume it, otherwise we are done.
        if (TokPrec < ExprPrec) 
            return LHS;

        // Okay, we know this is a binop
        int BinOp = CurTok;
        getNextToken(); // eat binop

        // Parse the primary expression after the binary operator.
        ExprAST *RHS = ParsePrimary();
        if (!RHS) return 0;

        // if BinOp 
        int Nextprec = GetTokPrecedence();
        if (TokPrec < Nextprec) {
            RHS = ParseBinOpRHS(TokPrec+1, RHS);
            if (RHS == 0) return 0;
        }

        // Merge LHS/RHS
        LHS = new BinaryExprAST(BinOp, LHS, RHS);
    }
}

// prototype 
//      ::= id '(' id* ')'

static PrototypeAST *ParseProtoype() {
    if (CurTok != tok_identifier)
        return ErrorP("Expected function name in prototype");

    std::string FnName = IdentifierStr;
    getNextToken();

    if (CurTok != '(')
        return ErrorP("Expected '(' in prototype");

    // Read the list of argument names.
    std::vector<std::string> ArgNames;
    while (getNextToken() == tok_identifier)
        ArgNames.push_back(IdentifierStr);

    if (CurTok != ')')
        return ErrorP("Expected ')' in prototype");

    // success
    getNextToken(); // eat ')'

    return new PrototypeAST(FnName, ArgNames);
}

// definition ::= 'def' prototype expression 

static FunctionAST *ParseDefinition() {
    getNextToken(); // eat def 
    PrototypeAST *Proto = ParseProtoype();
    if (Proto == 0) return 0;

    if (ExprAST *E = ParseExpression())
        return new FunctionAST(Proto, E);
    return 0;
}

// external ::= 'extern' prototype
static PrototypeAST *ParseExtern() {
    getNextToken(); // eat extern 
    return ParseProtoype();
}

// toplevelexpr ::= expression
static FunctionAST *ParseTopLevelExpr() {
    if (ExprAST *E = ParseExpression()) {
        PrototypeAST *Proto = new PrototypeAST("", std::vector<std::string>());
        return new FunctionAST(Proto, E);
    }
    return 0;
}

void HandleDefinition() {
    if (ParseDefinition()) {
        fprintf(stderr, "Parsed a function definition.\n");
    } else {
        // Skip token for error recovery
        getNextToken();
    }
}

void HandleExtern() {
    if(ParseExtern()) {
        fprintf(stderr, "Parsed an extern\n");
    } else {
        // Skip token for error recovery
        getNextToken();
    }
}

void HandleTopLevelExpression() {
    // Evaluate a top-level expression into an anonymous function 
    if (ParseTopLevelExpr()) {
        fprintf(stderr, "Parsed a top-level expression\n");
    } else {
        // Skip token for error recovery
        getNextToken();
    }
}
