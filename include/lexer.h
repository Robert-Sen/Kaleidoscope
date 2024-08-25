#ifndef LEXER_H
#define LEXER_H

#include <map>
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

  std::map<char, int> op_precedence_;

public:

  Lexer() = default;
  ~Lexer() = default;

  int get_next_tok();
  inline int get_cur_tok() { return cur_tok_; };
  template <typename T> T get_tok_value();

  void add_op(char op, int prec);
  int get_op_prec(char op);
};

#endif // LEXER_H
