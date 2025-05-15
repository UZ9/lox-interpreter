#include "parser.h"
#include "token.h"

static int is_digit(char c) { return c >= '0' && c <= '9'; }

static int is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static int is_alpha_numeric(char c) { return is_alpha(c) || is_digit(c); }

struct parser_t *parser_create() {
  struct parser_t *parser =
      (struct parser_t *)calloc(1, sizeof(struct parser_t));

  parser->tokens = arraylist_create(50);
  parser->keywords = hashmap_create(64);
  parser->line = 1;

  hashmap_put(parser->keywords, "and", AND);
  hashmap_put(parser->keywords, "class", CLASS);
  hashmap_put(parser->keywords, "else", ELSE);
  hashmap_put(parser->keywords, "false", FALSE);
  hashmap_put(parser->keywords, "for", FOR);
  hashmap_put(parser->keywords, "fun", FUN);
  hashmap_put(parser->keywords, "if", IF);
  hashmap_put(parser->keywords, "nil", NIL);
  hashmap_put(parser->keywords, "or", OR);
  hashmap_put(parser->keywords, "print", PRINT);
  hashmap_put(parser->keywords, "return", RETURN);
  hashmap_put(parser->keywords, "super", SUPER);
  hashmap_put(parser->keywords, "this", THIS);
  hashmap_put(parser->keywords, "true", TRUE);
  hashmap_put(parser->keywords, "var", VAR);
  hashmap_put(parser->keywords, "while", WHILE);

  return parser;
}

char parser_advance(struct parser_t *parser, char *file_contents) {
  return file_contents[parser->current_idx++];
}

static void parser_add_data_token(struct parser_t *parser, TokenType token,
                                  char *raw, void *data) {

  struct token_entry_t *entry = calloc(1, sizeof(struct token_entry_t));

  entry->data = data;
  entry->raw = raw;
  entry->type = token;

  arraylist_add(parser->tokens, (void *)entry);
}

static void parser_add_token(struct parser_t *parser, TokenType token) {
  parser_add_data_token(parser, token, NULL, NULL);
}

int parser_at_file_end(struct parser_t *parser, char *file_contents) {
  return file_contents[parser->current_idx] == '\0';
}

char parser_peek(struct parser_t *parser, char *file_contents) {
  if (parser_at_file_end(parser, file_contents))
    return '\0';

  return file_contents[parser->current_idx];
}

char parser_peek_next(struct parser_t *parser, char *file_contents) {
  if (parser_at_file_end(parser, file_contents + 1))
    return '\0';

  return file_contents[parser->current_idx + 1];
}

void parser_string(struct parser_t *parser, char *file_contents) {

  // until we reach another quotation mark, keep iterating
  while (parser_peek(parser, file_contents) != '"' &&
         !parser_at_file_end(parser, file_contents)) {
    if (parser_peek(parser, file_contents) == '\n')
      parser->line++;

    parser_advance(parser, file_contents);
  }

  if (parser_at_file_end(parser, file_contents)) {
    // reached end of file before quotation finished
    LOG_INTERPRETER_ERROR(parser, "Unterminated string.");
    return;
  }

  parser_advance(parser, file_contents); // get rid of last quotation mark

  // get substring

  int str_len = parser->current_idx - parser->start - 2;
  char *parsed_str = (char *)calloc(str_len, sizeof(char));

  strncpy(parsed_str, file_contents + parser->start + 1, str_len);

  // NOTE: could add escape sequences here in the future
  parser_add_data_token(parser, STRING, parsed_str, (void *)parsed_str);
}

int parser_match(struct parser_t *parser, char *file_contents, char desired) {
  if (parser_at_file_end(parser, file_contents))
    return 0;
  if (file_contents[parser->current_idx] != desired)
    return 0;

  parser->current_idx++;

  return 1;
}

void parser_number(struct parser_t *parser, char *file_contents) {
  // go through the entire number
  while (is_digit(parser_peek(parser, file_contents)))
    parser_advance(parser, file_contents);

  // number might be a decimal, if found discard and keep advancing
  if (parser_peek(parser, file_contents) == '.' &&
      is_digit(parser_peek_next(parser, file_contents))) {
    parser_advance(parser, file_contents); // discard .

    while (is_digit(parser_peek(parser, file_contents)))
      parser_advance(parser, file_contents);
  }

  int str_len = parser->current_idx - parser->start;
  char *parsed_str = (char *)calloc(str_len, sizeof(char));

  strncpy(parsed_str, file_contents + parser->start, str_len);

  double num = atof(parsed_str);

  // TODO: Find if there's a way to avoid having to move this to the heap
  double *num_heap = (double *)malloc(sizeof(double));
  *num_heap = num;

  parser_add_data_token(parser, NUMBER, parsed_str, (void *)num_heap);
}

void parser_identifier(struct parser_t *parser, char *file_contents) {
  while (is_alpha_numeric(parser_peek(parser, file_contents))) {
    parser_advance(parser, file_contents);
  }

  // retrieve string
  int str_len = parser->current_idx - parser->start;
  char *parsed_str = (char *)calloc(str_len, sizeof(char));

  strncpy(parsed_str, file_contents + parser->start, str_len);

  // check fo rreserved keyword
  TokenType type = hashmap_lookup(parser->keywords, parsed_str);

  // TODO: reserved
  parser_add_data_token(parser, type, parsed_str, parsed_str);
}

void parser_scan_token(struct parser_t *parser, char *file_contents) {
  char c = parser_advance(parser, file_contents);

  switch (c) {
  case ' ':
  case '\r':
  case '\t':
    break;

  case '\n':
    parser->line++;
    break;
  case '(':
    parser_add_token(parser, LEFT_PAREN);
    break;
  case ')':
    parser_add_token(parser, RIGHT_PAREN);
    break;
  case '{':
    parser_add_token(parser, LEFT_BRACE);
    break;
  case '}':
    parser_add_token(parser, RIGHT_BRACE);
    break;
  case ',':
    parser_add_token(parser, COMMA);
    break;
  case '.':
    parser_add_token(parser, DOT);
    break;
  case '-':
    parser_add_token(parser, MINUS);
    break;
  case '+':
    parser_add_token(parser, PLUS);
    break;
  case ';':
    parser_add_token(parser, SEMICOLON);
    break;
  case '*':
    parser_add_token(parser, STAR);
    break;
  case '!':
    parser_add_token(
        parser, parser_match(parser, file_contents, '=') ? BANG_EQUAL : BANG);
    break;
  case '=':
    parser_add_token(
        parser, parser_match(parser, file_contents, '=') ? EQUAL_EQUAL : EQUAL);
    break;
  case '>':
    parser_add_token(parser, parser_match(parser, file_contents, '=')
                                 ? GREATER_EQUAL
                                 : GREATER);
    break;
  case '<':
    parser_add_token(
        parser, (parser_match(parser, file_contents, '=') ? LESS_EQUAL : LESS));
    break;
  case '/':
    if (parser_match(parser, file_contents, '/')) {
      // detected comment, keep going
      while (parser_peek(parser, file_contents) != '\n' &&
             !parser_at_file_end(parser, file_contents))
        parser_advance(parser, file_contents);
    } else {
      parser_add_token(parser, SLASH);
    }

    break;

  case '"':
    parser_string(parser, file_contents);
    break;

  default:
    // check if we're dealing with a number
    if (is_digit(c)) {
      parser_number(parser, file_contents);
      return;
    }
    if (is_alpha(c)) {
      // check for keyword
      parser_identifier(parser, file_contents);
      return;
    }

    // syntax error
    LOG_INTERPRETER_ERROR(parser, "Unexpected character: %c", c);
    break;
  }
}

struct arraylist_t *parser_get_tokens(struct parser_t *parser) {
  return parser->tokens;
}

void parser_parse(struct parser_t *parser, char *contents) {
  if (strlen(contents) > 0) {
    while (!parser_at_file_end(parser, contents)) {
      parser->start = parser->current_idx;
      parser_scan_token(parser, contents);
    }
  }

  parser_add_token(parser, END_OF_FILE);
}
