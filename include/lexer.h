#ifndef LEXER_H
#define LEXER_H

#include <string>

enum TokenType {
  // End of file
  TOK_EOF = -1,

  // Keyword `def, for fuction declaration
  TOK_DEF = -2,

  // Keyword `extern, for call standard library function
  TOK_EXTERN = -3,

  // Identifier, [_a-zA-Z][_a-zA-Z0-9]*
  TOK_IDENTIFIER = -4,

  // Number, double value
  TOK_NUMBER = -5,
};

class Lexer {
  std::string identifier_str_;
  double number_val_;

  int cur_tok_;

public:
  Lexer() = default;
  ~Lexer() = default;

  int get_next_tok();
  inline int get_cur_tok() { return cur_tok_; };
  template <typename T> T get_tok_value();
};

#endif // LEXER_H
