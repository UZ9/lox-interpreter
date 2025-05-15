#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdint.h>

struct arraylist_t {
  uint32_t size;
  uint32_t capacity;
  void **data;
};

struct arraylist_t *arraylist_create(uint32_t initial_size);

void arraylist_add(struct arraylist_t *list, void *item);

void arraylist_add_at_index(struct arraylist_t *list, uint32_t index,
                            void *item);

void *arraylist_get(struct arraylist_t *list, uint32_t index);

void *arraylist_set(struct arraylist_t *list, uint32_t index, void *new_value);

void *arraylist_remove(struct arraylist_t *list, uint32_t index);

uint32_t arraylist_size(struct arraylist_t *list);

int arraylist_contains(struct arraylist_t *list, void *item);

int arraylist_index_of(struct arraylist_t *list, void *item);

int arraylist_is_empty(struct arraylist_t *list);

void arraylist_destroy(struct arraylist_t *list);

#endif // ARRAYLIST_H
