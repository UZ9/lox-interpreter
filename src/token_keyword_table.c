#include "token_keyword_table.h"

// basic, unsecure hash algorithm, would not use in production
// as it's susceptible to collision attacks
// hash function from https://benhoyt.com/writings/hash-table-in-c/
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define LOG_ERROR(msg, ...) fprintf(stderr, msg "\n", ##__VA_ARGS__)

static uint64_t hash_key(const char *key) {
  uint64_t hash = FNV_OFFSET;
  for (const char *p = key; *p; p++) {
    hash ^= (uint64_t)(unsigned char)(*p);
    hash *= FNV_PRIME;
  }
  return hash;
}

struct token_keyword_table *hashmap_create(uint32_t initial_capacity) {
  struct token_keyword_table *map = (struct token_keyword_table *)calloc(
      1, sizeof(struct token_keyword_table));

  map->size = 0;
  map->capacity = initial_capacity;
  map->entries = (struct token_keyword_table_entry_t *)calloc(
      initial_capacity, sizeof(struct token_keyword_table_entry_t));

  return map;
}

// add only hashmap; using this for constant lookup table
void hashmap_put(struct token_keyword_table *map, char *key, TokenType token) {
  // not dealing with any resize, as this will be a static lookup table
  if (map->size + 1 > map->capacity) {
    LOG_ERROR("hashmap_put: exceeded capacity of lookup table");
  }

  // bitmask trick: if capacity guaranteed power of 2,
  // we can just AND the other bits away
  // e.g. 64 = 1000000, we care about those zeroes
  // 64 - 1 = 63 = 0111111, use this as bitmask
  uint32_t initial_index = hash_key(key) & (map->capacity - 1);

  uint32_t index = initial_index;

  uint32_t probe_count = 1;

  while (map->entries[index].key != NULL) {
    // quadratic probing: 0, 1, 2, 4, 9, etc.
    // i = i_0 + probe_count^2
    index = initial_index + (probe_count * probe_count) & (map->capacity - 1);
    probe_count++;
  }

  // place new element
  map->entries[index].key = key;
  map->entries[index].type = token;

  map->size++;
}

TokenType hashmap_lookup(struct token_keyword_table *map, char *key) {

  uint32_t initial_index = hash_key(key) & (map->capacity - 1);

  uint32_t index = initial_index;

  uint32_t probe_count = 1;

  // TODO: infinite loop here unless there's an exit condiiton on high load
  // factors
  while (map->entries[index].key != NULL &&
         strcmp(map->entries[index].key, key) != 0 &&
         probe_count <= map->capacity) {
    // keys are not equal, but we can keep going
    index = initial_index + (probe_count * probe_count) & (map->capacity - 1);
    probe_count++;
  }

  if (map->entries[index].key == NULL || probe_count > map->capacity) {
    // didn't find key
    return NONE;
  } else {
    return map->entries[index].type;
  }
}

void hashmap_destroy(struct token_keyword_table *map) {
  // user is responsible for freeing items themselves
  free(map->entries);
  free(map);
}
