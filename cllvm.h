#ifndef _CLLVM_H_
#define _CLLVM_H_

#include "llvm/Analysis/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;
static Module *TheModule;
static IRBuilder<> Builder(getGlobalContext());
static std::map<std::string, Value*> NameValues;

#endif
