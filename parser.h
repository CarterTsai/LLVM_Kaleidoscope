#ifndef _PARSER_H_
#define _PARSER_H_ 

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include "lexer.h"

// BinopPrecedence - This holds the precedence for each binary operator that is
// defined
// std::map<char, int> BinopPrecedence;

// ExpreAST - Base class for all expression nodes

class ExprAST {
    public:
     virtual ~ExprAST() {}   
};

class NumberExprAST: public ExprAST {
    double Val;
public:
    NumberExprAST(double val): Val(val) {};
};

// VariableExprAST - Expression class for referencing a variable , like 'a'
class VariableExprAST: public ExprAST {
    std::string Name;
public:
    VariableExprAST(const std::string &name) : Name(name) {};
};

// BinaryExprAST - Expression class for a binary operator
class BinaryExprAST: public ExprAST {
    char Op;
    ExprAST *LHS, *RHS;
public:
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs) : Op(op), LHS(lhs),
    RHS(rhs) {};
};

// CallExprAST - Expression class for function calls
class CallExprAST : public ExprAST {
    std::string Callee;
    std::vector<ExprAST*> Args;
public:
    CallExprAST (const std::string &callee, std::vector<ExprAST*> &args) :
        Callee(callee), Args(args) {}
};

// PrototypeAST - This Class represents the "prototype" for a function,
// which captures its name, and its argument names ( thus implicitly the number
// of arguments the function takes).

class PrototypeAST {
    std::string Name;
    std::vector<std::string> Args;
public:
    PrototypeAST (const std::string &name, const std::vector<std::string> &args)
        : Name(name), Args(args) {};
};

// FunctionAST - This class represents a function definition itself.
class FunctionAST {
    PrototypeAST *Proto;
    ExprAST *Body;
public:
    FunctionAST (PrototypeAST *proto, ExprAST *body) : Proto(proto), 
        Body(body){};
};

// Function 

static ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS);
static ExprAST *ParseParenExpr();
static ExprAST *ParseNumberExpr();
static ExprAST *ParseIdentifierExpr();
ExprAST *Error(const char *Str);
PrototypeAST *ErrorP(const char *Str);
FunctionAST *ErrorF(const char *Str );

void HandleDefinition();
void HandleExtern();
void HandleTopLevelExpression();


#endif
