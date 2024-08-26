#include "parser.h"
#include "AST.h"
#include "lexer.h"

#include <error.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

///////////////////////////////////////////////////////////
// Parse primary
///////////////////////////////////////////////////////////

std::unique_ptr<NumberExprAST> Parser::parse_number_expr() {
  auto res =
      std::make_unique<NumberExprAST>(this->lexer_.get_tok_value<double>());
  this->lexer_.get_next_tok();
  return res;
}

std::unique_ptr<VariableExprAST> Parser::parse_variable_expr() {
  auto res = std::make_unique<VariableExprAST>(
      this->lexer_.get_tok_value<std::string_view>());
  this->lexer_.get_next_tok();
  return res;
}

std::unique_ptr<ExprAST> Parser::parse_paren_expr() {
  this->lexer_.get_next_tok(); // eat '('

  auto e = this->parse_expression();
  if (!e) {
    return nullptr;
  }

  if (this->lexer_.get_cur_tok() != ')') {
    return log_error("expected ')'");
  }

  this->lexer_.get_next_tok(); // eat ')'
  return e;
}

std::unique_ptr<ExprAST> Parser::parse_identifier_expr() {
  std::string id_name(this->lexer_.get_tok_value<std::string_view>());

  this->lexer_.get_next_tok(); // eat identifier

  // Simple variable
  if (this->lexer_.get_cur_tok() != '(') {
    return std::make_unique<VariableExprAST>(id_name);
  }

  // Function call
  this->lexer_.get_next_tok(); // eat '('
  std::vector<std::unique_ptr<ExprAST>> args;
  if (this->lexer_.get_cur_tok() != ')') {
    while (true) {
      if (auto arg = this->parse_expression()) {
        args.push_back(std::move(arg));
      } else {
        return nullptr;
      }

      if (this->lexer_.get_cur_tok() == ')')
        break;

      if (this->lexer_.get_cur_tok() != ',') {
        return log_error("expected ')' or ',' in argument list");
      }

      this->lexer_.get_next_tok(); // eat ','
    }
  }

  this->lexer_.get_next_tok(); // eat ')'
  return std::make_unique<CallExprAST>(id_name, std::move(args));
}

std::unique_ptr<ExprAST> Parser::parse_primary() {
  switch (this->lexer_.get_cur_tok()) {
  case TOK_IDENTIFIER:
    return this->parse_identifier_expr();
  case TOK_NUMBER:
    return this->parse_number_expr();
  case '(':
    return this->parse_paren_expr();
  default:
    return log_error("unknown token when expecting an expression");
  }
}

///////////////////////////////////////////////////////////
// Parse expression
///////////////////////////////////////////////////////////

std::unique_ptr<ExprAST> Parser::parse_expression() {
  auto lhs = this->parse_primary();
  if (!lhs) {
    return nullptr;
  }

  return parse_binop_rhs(0, std::move(lhs));
}

// The prec indicates the min precedence that the binary operators own in the
// rhs expression, to proceed parse_binop_rhs()
std::unique_ptr<ExprAST> Parser::parse_binop_rhs(int prec,
                                                 std::unique_ptr<ExprAST> lhs) {

  // todo
  while (true) {
    char cur_op = this->lexer_.get_cur_tok();
    int cur_prec = this->lexer_.get_op_prec(cur_op);

    if (cur_prec < prec) {
      return lhs;
    }

    this->lexer_.get_next_tok(); // eat op

    // Parse the primary expression after op
    auto rhs = this->parse_primary();
    if (!rhs) {
      return nullptr;
    }

    char next_op = this->lexer_.get_cur_tok();
    int next_prec = this->lexer_.get_op_prec(next_op);

    // Case: lhs cur_op (rhs next_op unparsed)
    if (cur_prec < next_prec) {
      rhs = this->parse_binop_rhs(cur_prec + 1, std::move(lhs));
      if (!rhs) {
        return nullptr;
      }
    }

    // Merge (lhs cur_op rhs)
    lhs =
        std::make_unique<BinaryExprAST>(cur_op, std::move(lhs), std::move(rhs));
  }
}

///////////////////////////////////////////////////////////
// Parse definition
///////////////////////////////////////////////////////////

std::unique_ptr<PrototypeAST> Parser::parse_prototype() {
  if (this->lexer_.get_cur_tok() != TOK_IDENTIFIER) {
    return log_error_proto("expected function name in prototype");
  }

  std::string fn_name(this->lexer_.get_tok_value<std::string_view>());

  this->lexer_.get_next_tok(); // eat `fn_name
  if (this->lexer_.get_cur_tok() != '(') {
    return log_error_proto("expected '(' in prototype");
  }

  std::vector<std::string> args;
  while (this->lexer_.get_next_tok() == TOK_IDENTIFIER) {
    args.push_back(std::string(this->lexer_.get_tok_value<std::string_view>()));
  }
  if (this->lexer_.get_cur_tok() != ')') {
    return log_error_proto("expected ')' in prototype");
  }

  this->lexer_.get_next_tok(); // eat ')'
  return std::make_unique<PrototypeAST>(fn_name, std::move(args));
}

std::unique_ptr<FunctionAST> Parser::parse_definition() {
  this->lexer_.get_next_tok(); // eat `def

  auto proto = this->parse_prototype();
  if (!proto) {
    return nullptr;
  }

  if (auto body = this->parse_expression()) {
    return std::make_unique<FunctionAST>(std::move(proto), std::move(body));
  }
  return nullptr;
}

std::unique_ptr<PrototypeAST> Parser::parse_extern() {
  this->lexer_.get_next_tok(); // eat `extern
  return this->parse_prototype();
}

std::unique_ptr<FunctionAST> Parser::parse_top_level_expr() {
  if (auto e = this->parse_expression()) {
    auto proto = std::make_unique<PrototypeAST>("__anony_expr",
                                                std::vector<std::string>());
    return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
  }
  return nullptr;
}

///////////////////////////////////////////////////////////
// Top level parsing
///////////////////////////////////////////////////////////

void Parser::handle_definition() {
  if (this->parse_definition()) {
    std::cout << "Parsed a function definition\n";
  } else {
    // Skip token for error recovery
    this->lexer_.get_next_tok();
  }
}

void Parser::handle_extern() {
  if (this->parse_extern()) {
    std::cout << "Parsed a extern function\n";
  } else {
    // Skip token for error recovery
    this->lexer_.get_next_tok();
  }
}

void Parser::handle_top_level_expresion() {
  if (parse_top_level_expr()) {
    std::cout << "Parsed a top level expression\n";
  } else {
    // Skip token for error recovery
    this->lexer_.get_next_tok();
  }
}