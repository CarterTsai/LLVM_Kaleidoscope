#ifndef ERROR_H
#define ERROR_H

// Error* - These are little helper function for error handling.
ExprAST *Error(const char *Str);
PrototypeAST *ErrorP(const char *Str);
FunctionAST *ErrorF(const char *Str );

#endif
