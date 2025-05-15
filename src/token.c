#include "token.h"
#include <stdio.h>

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

void print_keyword_token(char *name, struct token_entry_t *entry) {
  printf("%s %s null\n", name, (char *)entry->raw);
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
    print_keyword_token("AND", entry);
    break;
  case CLASS:
    print_keyword_token("CLASS", entry);
    break;
  case ELSE:
    print_keyword_token("ELSE", entry);
    break;
  case FALSE:
    print_keyword_token("FALSE", entry);
    break;
  case FUN:
    print_keyword_token("FUN", entry);
    break;
  case FOR:
    print_keyword_token("FOR", entry);
    break;
  case IF:
    print_keyword_token("IF", entry);
    break;
  case NIL:
    print_keyword_token("NIL", entry);
    break;
  case OR:
    print_keyword_token("OR", entry);
    break;
  case PRINT:
    print_keyword_token("PRINT", entry);
    break;
  case RETURN:
    print_keyword_token("RETURN", entry);
    break;
  case SUPER:
    print_keyword_token("SUPER", entry);
    break;
  case THIS:
    print_keyword_token("THIS", entry);
    break;
  case TRUE:
    print_keyword_token("TRUE", entry);
    break;
  case VAR:
    print_keyword_token("VAR", entry);
    break;
  case WHILE:
    print_keyword_token("WHILE", entry);
    break;
  case END_OF_FILE:
    printf("EOF  null\n");
    break;
  case NONE:
    printf("NONE null\n");
    break;
  }
}
