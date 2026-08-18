/* util.h
 *
 * Small, generic helpers shared across modules: checked allocation
 * wrappers (so every malloc()/realloc() failure is caught in one place)
 * and a helper for trimming line-ending characters read from a file or
 * stdin.
 */
#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

/* Allocates size bytes, exiting with an error message if the allocation
 * fails. Never returns NULL. */
void *safeMalloc(size_t size);

/* Resizes the block pointed to by pointer to size bytes, exiting with an
 * error message if the allocation fails. Never returns NULL. */
void *safeRealloc(void *pointer, size_t size);

/* Removes a trailing '\n' and/or '\r' from line, in place. */
void stripLineEnding(char *line);

#endif
