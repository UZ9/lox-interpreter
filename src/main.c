#include "arraylist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "token_keyword_table.h"

//
char *read_file_contents(const char *filename);

#define LOG_INTERPRETER_ERROR(msg, ...)                                        \
  fprintf(stderr, "[line %d] Error: " msg "\n", line, ##__VA_ARGS__);          \
  error = 1

// TODO: could potentially move this to a stack-allocated structure and almost
// completely avoid heap
static struct token_keyword_table *keywords;

struct token_entry_t {
  TokenType type;
  char *raw;
  void *data;
};

int file_contents_size = 0;
int current_idx = 0;
int line = 1;
int start = 0;
int error = 0;
struct arraylist_t *tokens = NULL;

int at_file_end(char *file_contents) {
  return file_contents[current_idx] == '\0';
}

char advance(char *file_contents) { return file_contents[current_idx++]; }

void add_data_token(TokenType token, char *raw, void *data) {

  struct token_entry_t *entry = calloc(1, sizeof(struct token_entry_t));

  entry->data = data;
  entry->raw = raw;
  entry->type = token;

  arraylist_add(tokens, (void *)entry);
}

void add_token(TokenType token) { add_data_token(token, NULL, NULL); }

int match(char *file_contents, char desired) {
  if (at_file_end(file_contents))
    return 0;
  if (file_contents[current_idx] != desired)
    return 0;

  current_idx++;

  return 1;
}

char peek(char *file_contents) {
  if (at_file_end(file_contents))
    return '\0';

  return file_contents[current_idx];
}

char peek_next(char *file_contents) {
  if (at_file_end(file_contents + 1))
    return '\0';

  return file_contents[current_idx + 1];
}

void string(char *file_contents) {

  // until we reach another quotation mark, keep iterating
  while (peek(file_contents) != '"' && !at_file_end(file_contents)) {
    if (peek(file_contents) == '\n')
      line++;

    advance(file_contents);
  }

  if (at_file_end(file_contents)) {
    // reached end of file before quotation finished
    LOG_INTERPRETER_ERROR("Unterminated string.");
    return;
  }

  advance(file_contents); // get rid of last quotation mark

  // get substring

  int str_len = current_idx - start - 2;
  char *parsed_str = (char *)calloc(str_len, sizeof(char));

  strncpy(parsed_str, file_contents + start + 1, str_len);

  // NOTE: could add escape sequences here in the future
  add_data_token(STRING, parsed_str, (void *)parsed_str);
}

int is_digit(char c) { return c >= '0' && c <= '9'; }

int is_alpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

void identifier(char *file_contents) {
  while (is_alpha(peek(file_contents))) {
    advance(file_contents);
  }

  // retrieve string
  int str_len = current_idx - start;
  char *parsed_str = (char *)calloc(str_len, sizeof(char));

  strncpy(parsed_str, file_contents + start, str_len);

  // TODO: reserved
  add_data_token(IDENTIFIER, parsed_str, parsed_str);
}

void number(char *file_contents) {
  // go through the entire number
  while (is_digit(peek(file_contents)))
    advance(file_contents);

  // number might be a decimal, if found discard and keep advancing
  if (peek(file_contents) == '.' && is_digit(peek_next(file_contents))) {
    advance(file_contents); // discard .

    while (is_digit(peek(file_contents)))
      advance(file_contents);
  }

  int str_len = current_idx - start;
  char *parsed_str = (char *)calloc(str_len, sizeof(char));

  strncpy(parsed_str, file_contents + start, str_len);

  double num = atof(parsed_str);

  // TODO: Find if there's a way to avoid having to move this to the heap
  double *num_heap = (double *)malloc(sizeof(double));
  *num_heap = num;

  add_data_token(NUMBER, parsed_str, (void *)num_heap);
}

void print_number_token(struct token_entry_t *entry) {
  // this is far from ideal, but the tests require printing an int as x.0, and
  // is not the default C behavior (this was intended for java)
  // instead, check if it's an int and print accordingly

  double num = *((double *)(entry->data));

  if (((int)(num)) == num) {
    // int
    printf("NUMBER %s %d.0\n", entry->raw, (int)num);
  } else {
    printf("NUMBER %s %s\n", entry->raw, entry->raw);
  }
}

void print_token_entry(struct token_entry_t *entry) {
  // the annoying part about c...
  switch (entry->type) {

  case LEFT_PAREN:
    printf("LEFT_PAREN ( null\n");
    break;
  case RIGHT_PAREN:
    printf("RIGHT_PAREN ) null\n");
    break;
  case LEFT_BRACE:
    printf("LEFT_BRACE { null\n");
    break;
  case RIGHT_BRACE:
    printf("RIGHT_BRACE } null\n");
    break;
  case COMMA:
    printf("COMMA , null\n");
    break;
  case DOT:
    printf("DOT . null\n");
    break;
  case MINUS:
    printf("MINUS - null\n");
    break;
  case PLUS:
    printf("PLUS + null\n");
    break;
  case SEMICOLON:
    printf("SEMICOLON ; null\n");
    break;
  case SLASH:
    printf("SLASH / null\n");
    break;
  case STAR:
    printf("STAR * null\n");
    break;
  case BANG:
    printf("BANG ! null\n");
    break;
  case BANG_EQUAL:
    printf("BANG_EQUAL != null\n");
    break;
  case EQUAL:
    printf("EQUAL = null\n");
    break;
  case EQUAL_EQUAL:
    printf("EQUAL_EQUAL == null\n");
    break;
  case GREATER:
    printf("GREATER > null\n");
    break;
  case GREATER_EQUAL:
    printf("GREATER_EQUAL >= null\n");
    break;
  case LESS:
    printf("LESS < null\n");
    break;
  case LESS_EQUAL:
    printf("LESS_EQUAL <= null\n");
    break;
  case IDENTIFIER:
    printf("IDENTIFIER %s null\n", (char *)entry->raw);
    break;
  case STRING:
    printf("STRING \"%s\" %s\n", (char *)entry->data, (char *)entry->data);
    break;
  case NUMBER:
    print_number_token(entry);
    break;
  case AND:
    printf("AND null\n");
    break;
  case CLASS:
    printf("CLASS null\n");
    break;
  case ELSE:
    printf("ELSE null\n");
    break;
  case FALSE:
    printf("FALSE null\n");
    break;
  case FUN:
    printf("FUN null\n");
    break;
  case FOR:
    printf("FOR null\n");
    break;
  case IF:
    printf("IF null\n");
    break;
  case NIL:
    printf("NIL null\n");
    break;
  case OR:
    printf("OR null\n");
    break;
  case PRINT:
    printf("PRINT null\n");
    break;
  case RETURN:
    printf("RETURN null\n");
    break;
  case SUPER:
    printf("SUPER null\n");
    break;
  case THIS:
    printf("THIS null\n");
    break;
  case TRUE:
    printf("TRUE null\n");
    break;
  case VAR:
    printf("VAR null\n");
    break;
  case WHILE:
    printf("WHILE null\n");
    break;
  case END_OF_FILE:
    printf("EOF  null\n");
    break;
  case NONE:
    printf("NONE null\n");
    break;
  }
}

void scan_token(char *file_contents) {
  char c = advance(file_contents);

  switch (c) {
  case ' ':
  case '\r':
  case '\t':
    break;

  case '\n':
    line++;
    break;
  case '(':
    add_token(LEFT_PAREN);
    break;
  case ')':
    add_token(RIGHT_PAREN);
    break;
  case '{':
    add_token(LEFT_BRACE);
    break;
  case '}':
    add_token(RIGHT_BRACE);
    break;
  case ',':
    add_token(COMMA);
    break;
  case '.':
    add_token(DOT);
    break;
  case '-':
    add_token(MINUS);
    break;
  case '+':
    add_token(PLUS);
    break;
  case ';':
    add_token(SEMICOLON);
    break;
  case '*':
    add_token(STAR);
    break;
  case '!':
    add_token(match(file_contents, '=') ? BANG_EQUAL : BANG);
    break;
  case '=':
    add_token(match(file_contents, '=') ? EQUAL_EQUAL : EQUAL);
    break;
  case '>':
    add_token(match(file_contents, '=') ? GREATER_EQUAL : GREATER);
    break;
  case '<':
    add_token(match(file_contents, '=') ? LESS_EQUAL : LESS);
    break;
  case '/':
    if (match(file_contents, '/')) {
      // detected comment, keep going
      while (peek(file_contents) != '\n' && !at_file_end(file_contents))
        advance(file_contents);
    } else {
      add_token(SLASH);
    }

    break;

  case '"':
    string(file_contents);
    break;

  default:
    // check if we're dealing with a number
    if (is_digit(c)) {
      number(file_contents);
      return;
    }
    if (is_alpha(c)) {
      // check for keyword
      identifier(file_contents);
      return;
    }

    // syntax error
    LOG_INTERPRETER_ERROR("Unexpected character: %c", c);
    break;
  }
}

void initialize_tokenizer() {
  tokens = arraylist_create(50);

  keywords = hashmap_create(64);

  hashmap_put(keywords, "and", AND);
  hashmap_put(keywords, "class", CLASS);
  hashmap_put(keywords, "else", ELSE);
  hashmap_put(keywords, "false", FALSE);
  hashmap_put(keywords, "for", FOR);
  hashmap_put(keywords, "fun", FUN);
  hashmap_put(keywords, "if", IF);
  hashmap_put(keywords, "nil", NIL);
  hashmap_put(keywords, "or", OR);
  hashmap_put(keywords, "print", PRINT);
  hashmap_put(keywords, "return", RETURN);
  hashmap_put(keywords, "super", SUPER);
  hashmap_put(keywords, "this", THIS);
  hashmap_put(keywords, "true", TRUE);
  hashmap_put(keywords, "var", VAR);
  hashmap_put(keywords, "while", WHILE);
}

int main(int argc, char *argv[]) {
  // Disable output buffering
  setbuf(stdout, NULL);
  setbuf(stderr, NULL);

  initialize_tokenizer();

  if (argc < 3) {
    fprintf(stderr, "Usage: ./your_program tokenize <filename>\n");
    return 1;
  }

  const char *command = argv[1];

  if (strcmp(command, "tokenize") == 0) {
    // You can use print statements as follows for debugging, they'll be visible
    // when running tests.
    fprintf(stderr, "Logs from your program will appear here!\n");

    char *file_contents = read_file_contents(argv[2]);

    file_contents_size = strlen(file_contents);

    // Uncomment this block to pass the first stage
    if (strlen(file_contents) > 0) {
      while (!at_file_end(file_contents)) {
        start = current_idx;
        scan_token(file_contents);
      }
    }

    add_token(END_OF_FILE);

    for (int i = 0; i < tokens->size; i++) {
      print_token_entry((struct token_entry_t *)tokens->data[i]);
    }

    free(file_contents);
  } else {
    fprintf(stderr, "Unknown command: %s\n", command);
    return 1;
  }

  if (error) {
    return 65;
  }

  return 0;
}

char *read_file_contents(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "Error reading file: %s\n", filename);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);

  char *file_contents = malloc(file_size + 1);
  if (file_contents == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(file_contents, 1, file_size, file);
  if (bytes_read < file_size) {
    fprintf(stderr, "Error reading file contents\n");
    free(file_contents);
    fclose(file);
    return NULL;
  }

  file_contents[file_size] = '\0';
  fclose(file);

  return file_contents;
}
