#include "util/vector.h"

Vector *initialize_vector(char *type) {
    Vector *vector = malloc(sizeof(*vector));

    if (!strcmp(type, "char")) {
        vector->base = malloc(sizeof(char) * VECTOR_BASE_SIZE);
        vector->type_size = sizeof(char);
    } else {
        kill_vector(vector); // dk why, but don't care
        assert(0 && "CANNOT INITIALIZE A VECTOR WITH THAT TYPE!");
    }

    vector->len = 0;
    vector->size = 2;

    return vector;
}

void vec_push_back(Vector *vector, void *element) {
    if (vector->len == vector->size) {
        vector->size *= 2;
        vector->base = realloc(vector->base, vector->size * vector->type_size);

        if (!vector->base)
            assert(0 && "vector realloc failed");
    }

    if (vector->type_size == sizeof(char) && vector->len > 0) {
        // Go back one step to override the last null terminator
        vector->len--;
    }

    memcpy((char *)vector->base + vector->len * vector->type_size, element, vector->type_size);
    vector->len++;

    if (vector->type_size == sizeof(char)) {
        // always append a null-terminated at the end
        char zero = '\0';
        memcpy((char *)vector->base + vector->len * vector->type_size, &zero, vector->type_size);
        vector->len++;
    }
}

void vec_clear(Vector *vector) {
    if (!vector) return;

    vector->len = 0;

    if (vector->type_size == sizeof(char)) {
        char ch = '\0';

        memcpy((char *)vector->base, &ch, vector->type_size);
    }
}

void kill_vector(Vector *vector) {
    if (!vector) return;

    free(vector->base);
    free(vector);
}
