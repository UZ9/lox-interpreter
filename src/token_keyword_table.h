#ifndef TOKEN_KEYWORD_TABLE_H
#define TOKEN_KEYWORD_TABLE_H

#include "token.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct token_keyword_table_entry_t {
  char *key;
  TokenType type;
};

struct token_keyword_table {
  struct token_keyword_table_entry_t *entries;
  uint32_t size;
  uint32_t capacity;
};

struct token_keyword_table *hashmap_create(uint32_t initial_capacity);

void hashmap_put(struct token_keyword_table *map, char *key, TokenType token);

TokenType hashmap_lookup(struct token_keyword_table *map, char *key);

void hashmap_destroy(struct token_keyword_table *map);

#endif // TOKEN_KEYWORD_TABLE_H
