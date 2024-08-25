#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "lexer.h"

#include <memory>
#include <string>

class Parser {
  Lexer lexer_;

public:
  Parser() = default;
  ~Parser() = default;

  std::unique_ptr<NumberExprAST> parse_number_expr();
  std::unique_ptr<VariableExprAST> parse_variable_expr();
  std::unique_ptr<ExprAST> parse_paren_expr();
  std::unique_ptr<ExprAST> parse_identifier_expr();
  std::unique_ptr<ExprAST> parse_primary();

  std::unique_ptr<ExprAST> parse_expression();
  std::unique_ptr<ExprAST> parse_binop_rhs(int prec, std::unique_ptr<ExprAST> lhs);

  std::unique_ptr<PrototypeAST> parse_prototype();
  std::unique_ptr<FunctionAST> parse_definition();
  std::unique_ptr<PrototypeAST> parse_extern();
  std::unique_ptr<FunctionAST> parse_top_level_expr();

  void handle_definition();
  void handle_extern();
  void handle_top_level_expresion();
};

#endif