#include "lexer.h"

#include <assert.h>
#include <iostream>
#include <string_view>

int main() {
  Lexer lexer;

  while (true) {
    lexer.get_next_tok();
    switch (lexer.get_cur_tok()) {
    case TOK_EOF:
      std::cout << "TOK_EOF\n";
      break;
    case TOK_DEF:
      std::cout << "TOK_DEF: " << lexer.get_tok_value<std::string_view>()
                << '\n';
      break;
    case TOK_EXTERN:
      std::cout << "TOK_EXTERN: " << lexer.get_tok_value<std::string_view>()
                << '\n';
      break;
    case TOK_IDENTIFIER:
      std::cout << "TOK_IDENTIFIER: " << lexer.get_tok_value<std::string_view>()
                << '\n';
      break;
    case TOK_NUMBER:
      std::cout << "TOK_NUMBER: " << lexer.get_tok_value<double>()
                << '\n';
      break;
    default:
      std::cerr << "Unknown token\n";
    }
  }

  return 0;
}