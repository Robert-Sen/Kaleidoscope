//
// Created by wss on 8/25/24.
//

#include "lexer.h"
#include <string_view>

int Lexer::get_next_tok() {
  static int last_char = ' ';

  while (std::isspace(last_char)) {
    last_char = std::getchar();
  }

  // Identifier
  if (last_char == '_' || std::isalpha(last_char)) {
    this->identifier_str_ = last_char;
    while (last_char == '_' || std::isalpha(last_char) ||
           std::isdigit(last_char)) {
      last_char = std::getchar();
      this->identifier_str_.push_back(last_char);
    }

    if (this->identifier_str_ == "def") {
      return this->cur_tok_ = TOK_DEF;
    }
    if (this->identifier_str_ == "extern") {
      return this->cur_tok_ = TOK_EXTERN;
    }
    return this->cur_tok_ = TOK_IDENTIFIER;
  }

  // Number
  if (std::isdigit(last_char) || last_char == '.') {
    std::string num_str;

    do {
      num_str.push_back(last_char);
      last_char = std::getchar();
    } while (std::isdigit(last_char) || last_char == '.');

    this->number_val_ = std::strtod(num_str.c_str(), 0);
    return this->cur_tok_ = TOK_NUMBER;
  }

  // Comment
  if (last_char == '#') {
    while (last_char != EOF && last_char != '\n' && last_char != '\r') {
      last_char = std::getchar();
    }

    if (last_char != EOF) {
      return this->get_next_tok();
    }
  }

  // EOF
  if (last_char == EOF) {
    return this->cur_tok_ = TOK_EOF;
  }

  // Unknown token, just return the charactor as its ascii value
  int this_char = last_char;
  last_char = std::getchar();
  return this_char;
}

template <> std::string_view Lexer::get_tok_value<std::string_view>() {
  return this->identifier_str_;
}

template <> double Lexer::get_tok_value<double>() { return this->number_val_; }