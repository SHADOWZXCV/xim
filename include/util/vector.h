#ifndef VECTOR_H_
#define VECTOR_H_
#define VECTOR_BASE_SIZE 2
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

typedef struct {
    void *base; // any type
    size_t size;
    size_t len;
    size_t type_size;
} Vector;

Vector *initialize_vector(char *type);
void vec_push_back(Vector *vector, void *element);
void kill_vector(Vector *vector);
void vec_clear(Vector *vector);

#endif
