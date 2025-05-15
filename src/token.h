#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>

typedef enum {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,

  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,

  IDENTIFIER,
  STRING,
  NUMBER,

  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,

  // renamed to end_of_file to not conflict with stdio
  END_OF_FILE,
  NONE
} TokenType;

struct token_keyword_table_entry_t {
  char *key;
  TokenType type;
};

struct token_keyword_table {
  struct token_keyword_table_entry_t *entries;
  uint32_t size;
  uint32_t capacity;
};

struct token_entry_t {
  TokenType type;
  char *raw;
  void *data;
};

void print_number_token(struct token_entry_t *entry);

void print_keyword_token(char *name, struct token_entry_t *entry);

void print_token_entry(struct token_entry_t *entry);

#endif // TOKEN_H
