#ifndef AST_H
#define AST_H

#include <llvm/IR/Function.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class ExprAST {

public:
  // ExprAST() = default;
  virtual ~ExprAST() = default;

  virtual llvm::Value *gen_ir() = 0;
};

///////////////////////////////////////////////////////////
// Expression AST
///////////////////////////////////////////////////////////

// NumberExprAST
class NumberExprAST : public ExprAST {
  double val_;

public:
  explicit NumberExprAST(double val) : val_(val) {}

  llvm::Value *gen_ir() override;
};

// VariableExprAST
class VariableExprAST : public ExprAST {
  std::string name_;

public:
  explicit VariableExprAST(std::string_view name) : name_(name) {}

  llvm::Value *gen_ir() override;
};

// BinaryExprAST
class BinaryExprAST : public ExprAST {
  int op_;
  std::unique_ptr<ExprAST> lhs_, rhs_;

public:
  BinaryExprAST(int op, std::unique_ptr<ExprAST> lhs,
                std::unique_ptr<ExprAST> rhs)
      : op_(op), lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

  llvm::Value *gen_ir() override;
};

// CallExprAST
class CallExprAST : public ExprAST {
  std::string callee_;
  std::vector<std::unique_ptr<ExprAST>> args_;

public:
  CallExprAST(std::string_view callee,
              std::vector<std::unique_ptr<ExprAST>> args)
      : callee_(callee), args_(std::move(args)) {}

  llvm::Value *gen_ir() override;
};

///////////////////////////////////////////////////////////
// Type Definition AST
///////////////////////////////////////////////////////////

// PrototypeAST
class PrototypeAST : public ExprAST {
  std::string name_;
  std::vector<std::string> args_;

public:
  PrototypeAST(std::string_view name, std::vector<std::string> args)
      : name_(name), args_(std::move(args)) {}

  std::string_view get_name() { return name_; }

  llvm::Function *gen_ir() override;
};

class FunctionAST : public ExprAST {
  std::unique_ptr<PrototypeAST> proto_;
  std::unique_ptr<ExprAST> body_;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> proto,
              std::unique_ptr<ExprAST> body)
      : proto_(std::move(proto)), body_(std::move(body)) {}

  llvm::Function *gen_ir() override;
};

#endif