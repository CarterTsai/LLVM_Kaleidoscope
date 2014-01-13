#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include "cllvm.h"
#include "parser.h"
#include "error.h"

double NumVal;
int CurTok;
std::string IdentifierStr;
std::map<char, int> BinopPrecedence; 


ExprAST *ParsePrimary() {
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

ExprAST *ParseExpression() {
    ExprAST *LHS = ParsePrimary();
    if (!LHS) return 0;

    return ParseBinOpRHS(0, LHS);
}

// numberexpre ::= number
ExprAST *ParseNumberExpr() {
    ExprAST *Result = new NumberExprAST(NumVal);
    getNextToken();
    return Result;
}

// parenexpr ::= '(' expression ')'
ExprAST *ParseParenExpr() {
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
ExprAST *ParseIdentifierExpr() {
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
ExprAST *ParseBinOpRHS(int ExprPrec, ExprAST *LHS) {
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

// Code Generation

Value *NumberExprAST::Codegen() {
    return ConstantFP::get(getGlobalContext(), APFloat(Val));
}

Value *VariableExprAST::Codegen() {
    // Look this variable up in the function
    Value *V = NameValues[Name];
    return V ? V : ErrorV("Unknown variable name");
}

Value *BinaryExprAST::Codegen() {
    Value *L = LHS->Codegen();
    Value *R = RHS->Codegen();
    if (L == 0 || R ==0) return 0;
    
    switch(Op) {
        case '+': return Builder.CreateFAdd(L, R, "addtmp");
        case '-': return Builder.CreateFSub(L, R, "subtmp");
        case '*': return Builder.CreateFMul(L, R, "multmp");
        case '<':
                  L = Builder.CreateFCmpULT(L, R, "cmptmp");
                  // Convert bool 0/1 to double 0.0 or 1.0
                  return Builder.CreateUIToFP(L, Type::getDoubleTy(
                          getGlobalContext()), "booltmp");
        default: return ErrorV("invalid binary operator");
    }       
}

Value *CallExprAST::Codegen() {
    // Look up the name in the global module table.
    Function *CalleeF = TheModule->getFunction(Callee);
    if(CalleeF == 0)
        return ErrorV("Unknown function referenced");

    // If argument mismatch error.
    if(CalleeF->arg_size() != Args.size())
        return ErrorV("Incorrect # arguments passed");

    std::vector<Value*> ArgsV;
    for (unsigned i = 0, e = Args.size(); i != e ; ++i) {
        ArgsV.push_back(Args[i]->Codegen());
        if (ArgsV.back() == 0) return 0;
    }

    return Builder.CreateCall(CalleeF, ArgsV, "calltmp");
}

Function *PrototypeAST::Codegen() {
    // Make the function type: double(double, double) etc.
    std::vector<Type*> Doubles( Args.size(),
                                Type::getDoubleTy(getGlobalContext()));
    FunctionType *FT = FunctionType::get(Type::getDoubleTy(getGlobalContext()),
                                     Doubles, false);
    Function *F = Function::Create(FT, Function::ExternalLinkage, Name, TheModule);

    // If F conflicted, there was already something name 'Name'. If it has a
    // body, don't allow redefinition or reextern
    if (F->getName() != Name) {
        // Delete the one we just made and get the existing one.
        F->eraseFromParent();
        F = TheModule->getFunction(Name);
        
        // If F already has a body, reject this.
        if (!F->empty()) {
            ErrorF("redefinition of function");
            return 0;
        }

        // if F took a different number of args, reject.
        if (F->arg_size() != Args.size()) {
            ErrorF("redefinition of function with differnet # args");
            return 0;
        }
    }
    // Set name for all argument.
    unsigned Idx = 0;
    for (Function::arg_iterator AI = F->arg_begin(); Idx != Args.size();
        ++AI, ++Idx) {
        AI->setName(Args[Idx]);
        // Add arguments to variable symbol table.
        NameValues[Args[Idx]] = AI;
    }
    return F;
}

Function *FunctionAST::Codegen() {
    NameValues.clear();

    Function *TheFunction = Proto->Codegen();       
    if (TheFunction == 0) {
        return 0;
    }   

    // Create a new basic block to start insertion into.
    BasicBlock *BB = BasicBlock::Create(getGlobalContext(), "entry", TheFunction);
    Builder.SetInsertPoint(BB);
    if (Value *RetVal = Body->Codegen()) {
        if (Value *RetVal = Body->Codegen()) {
            // Finish off the function.
            Builder.CreateRet(RetVal);

            // Validate the generated code, checking for consistency
            verifyFunction(*TheFunction);

            return TheFunction;
        }
        // Error reading body, remove function
        TheFunction->eraseFromParent();
        return 0;   
    }
    return 0;
}
