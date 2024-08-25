#include "error.h"
#include "AST.h"

#include <memory>

std::unique_ptr<ExprAST> log_error(const char *str) {
  std::cerr << str << '\n';
  return nullptr;
}

std::unique_ptr<PrototypeAST> log_error_proto(const char *str) {
  return log_error_proto(str);
}