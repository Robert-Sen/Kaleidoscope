#include "lexer.h"
#include "parser.h"

#include <iostream>

int main() {
  Parser parser;

  parser.lexer_.add_op('>', 10);
  parser.lexer_.add_op('+', 20);
  parser.lexer_.add_op('-', 20);
  parser.lexer_.add_op('*', 40);

  std::cout << "ready> ";
  parser.lexer_.get_next_tok();

  while (true) {
    std::cout << "ready> ";
    switch (parser.lexer_.get_cur_tok()) {
    case TOK_EXTERN:
      parser.handle_extern();
      break;
    case TOK_DEF:
      parser.handle_definition();
      break;
    case ';':
      parser.lexer_.get_next_tok(); // eat ';'
      break;
    case TOK_EOF:
      return 0;
    default:
      parser.handle_top_level_expresion();
    }
  }

  return 0;
}