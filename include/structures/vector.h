#ifndef VECTOR_H_
#define VECTOR_H_
#define VECTOR_BASE_SIZE 2
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include "types.h"

typedef struct {
    void *base; // any type
    size_t size;
    size_t len;
    size_t type_size;
    enum DATA_TYPES type;
} Vector;

Vector *initialize_vector(char *type, size_t type_size);
void vec_push_back(Vector *vector, void *element);
void free_vector(Vector *vector);
void vec_clear(Vector *vector);

#endif
