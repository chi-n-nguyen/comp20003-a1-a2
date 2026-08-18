/* util.c - see util.h for the module's interface documentation. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

void *safeMalloc(size_t size) {
    void *pointer = malloc(size);
    if (pointer == NULL) {
        fprintf(stderr, "Error: failed to allocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return pointer;
}

void *safeRealloc(void *pointer, size_t size) {
    void *newPointer = realloc(pointer, size);
    if (newPointer == NULL) {
        fprintf(stderr, "Error: failed to reallocate %zu bytes.\n", size);
        exit(EXIT_FAILURE);
    }
    return newPointer;
}

void stripLineEnding(char *line) {
    size_t length = strlen(line);
    while (length > 0 &&
           (line[length - 1] == '\n' || line[length - 1] == '\r')) {
        line[length - 1] = '\0';
        length--;
    }
}
