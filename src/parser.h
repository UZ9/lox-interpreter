#ifndef PARSER_H
#define PARSER_H

#include "arraylist.h"
#include "token.h"
#include "token_keyword_table.h"

#define LOG_INTERPRETER_ERROR(parser, msg, ...)                                \
  fprintf(stderr, "[line %d] Error: " msg "\n", parser->line, ##__VA_ARGS__);  \
  parser->error = 1

struct parser_t {
  struct arraylist_t *tokens;
  struct token_keyword_table *keywords;
  uint32_t line;
  uint8_t error;
  uint32_t start;
  uint32_t current_idx;
};

struct parser_t *parser_create();

char parser_advance(struct parser_t *parser, char *file_contents);

static void parser_add_data_token(struct parser_t *parser, TokenType token,
                                  char *raw, void *data);

static void parser_add_token(struct parser_t *parser, TokenType token);

int parser_at_file_end(struct parser_t *parser, char *file_contents);

char parser_peek(struct parser_t *parser, char *file_contents);

char parser_peek_next(struct parser_t *parser, char *file_contents);

void parser_string(struct parser_t *parser, char *file_contents);

int parser_match(struct parser_t *parser, char *file_contents, char desired);

void parser_number(struct parser_t *parser, char *file_contents);

void parser_identifier(struct parser_t *parser, char *file_contents);

void parser_parse(struct parser_t *parser, char *contents);

void parser_scan_token(struct parser_t *parser, char *file_contents);

struct arraylist_t *parser_get_tokens(struct parser_t *parser);

#endif // PARSER_H
