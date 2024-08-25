#ifndef ERROR_H
#define ERROR_H

#include "AST.h"

#include <iostream>
#include <memory>

std::unique_ptr<ExprAST> log_error(const char *str);
std::unique_ptr<PrototypeAST> log_error_proto(const char *str);

#endif // !ERROR_H