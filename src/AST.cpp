#include "AST.h"
#include "error.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

static std::unique_ptr<llvm::LLVMContext> kContext =
    std::make_unique<llvm::LLVMContext>();
static auto kBuilder = std::make_unique<llvm::IRBuilder<>>(*kContext);
static auto kModule =
    std::make_unique<llvm::Module>("default_module", *kContext);
static std::map<std::string, llvm::Value *> kNamedValues;

llvm::Value *NumberExprAST::gen_ir() {
  return llvm::ConstantFP::get(*kContext, llvm::APFloat(this->val_));
}

llvm::Value *VariableExprAST::gen_ir() {
  llvm::Value *v = kNamedValues[this->name_];
  if (!v) {
    log_error_value("unknown variable value");
  }
  return v;
}

llvm::Value *BinaryExprAST::gen_ir() {
  auto *lv = this->lhs_->gen_ir();
  auto *rv = this->rhs_->gen_ir();

  if (!lv || !rv) {
    return nullptr;
  }

  switch (this->op_) {
  case '+':
    return kBuilder->CreateAdd(lv, rv, "addtmp");
  case '-':
    return kBuilder->CreateSub(lv, rv, "subtmp");
  case '*':
    return kBuilder->CreateMul(lv, rv, "multmp");
  case '/':
    return kBuilder->CreateFDiv(lv, rv, "divtmp");
  case '>':
    lv = kBuilder->CreateFCmpUGT(lv, rv, "cmptmp");
    return kBuilder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*kContext));
  case '<':
    lv = kBuilder->CreateFCmpULT(lv, rv, "cmptmp");
    return kBuilder->CreateUIToFP(lv, llvm::Type::getDoubleTy(*kContext));

  default:
    return log_error_value("invalid binary operator");
  }
}

llvm::Value *CallExprAST::gen_ir() {
  llvm::Function *callee = kModule->getFunction(this->callee_);
  if (!callee) {
    return log_error_value("unknown function referenced");
  }

  if (callee->arg_size() != this->args_.size()) {
    return log_error_value("incorrect # arguments passed");
  }

  std::vector<llvm::Value *> args_v;
  for (const auto &arg : this->args_) {
    auto v = arg->gen_ir();
    if (!v) {
      return nullptr;
    }
    args_v.push_back(v);
  }

  return kBuilder->CreateCall(callee, std::move(args_v));
}

llvm::Function *PrototypeAST::gen_ir() {
  // Make the fuction type: double(double, double ... )
  std::vector<llvm::Type *> args_type(this->args_.size(),
                                      llvm::Type::getDoubleTy(*kContext));

  llvm::FunctionType *ft = llvm::FunctionType::get(
      llvm::Type::getDoubleTy(*kContext), args_type, false);

  llvm::Function *f = llvm::Function::Create(
      ft, llvm::Function::ExternalLinkage, this->name_, kModule.get());

  // Set names for all arguments
  unsigned idx = 0;
  for (auto &arg : f->args()) {
    arg.setName(this->args_[idx++]);
  }

  return f;
}

llvm::Function *FunctionAST::gen_ir() {
  // Look for an existing function from function table
  llvm::Function *func = kModule->getFunction(this->proto_->get_name());

  if (!func) {
    func = this->proto_->gen_ir();
  }

  if (!func) {
    return nullptr;
  }

  // Assert the function has no body
  if (!func->empty()) {
    return (llvm::Function *)log_error_value("function cannot be redefinited");
  }

  llvm::BasicBlock *bb = llvm::BasicBlock::Create(*kContext, "entry", func);
  kBuilder->SetInsertPoint(bb);

  // Record the function arguments in the kNamedValues map
  kNamedValues.clear();
  for (auto &arg : func->args()) {
    kNamedValues[std::string(arg.getName())] = &arg;
  }

  if (llvm::Value *ret_val = body_->gen_ir()) {
    kBuilder->CreateRet(ret_val);

    llvm::verifyFunction(*func);

    return func;
  }
  func->eraseFromParent();
  return nullptr;
}
