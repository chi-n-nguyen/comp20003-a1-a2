#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csv.h"
#include "util.h"

static void storeField(char *fields[], int *fieldCount, int maxFields,
                        const char *buffer, int bufferLength) {
    if (*fieldCount >= maxFields) {
        fprintf(stderr, "Error: CSV line has more than %d fields.\n", maxFields);
        exit(EXIT_FAILURE);
    }

    char *field = safeMalloc(bufferLength + 1);
    memcpy(field, buffer, bufferLength);
    field[bufferLength] = '\0';

    fields[*fieldCount] = field;
    (*fieldCount)++;
}

int csvSplitLine(const char *line, char *fields[], int maxFields) {
    size_t lineLength = strlen(line);
    /* A field can never be longer than the line it came from. */
    char *buffer = safeMalloc(lineLength + 1);
    int bufferLength = 0;
    int fieldCount = 0;
    int inQuotes = 0;
    size_t i;

    for (i = 0; i < lineLength; i++) {
        char c = line[i];

        if (inQuotes) {
            if (c == '"' && line[i + 1] == '"') {
                buffer[bufferLength++] = '"';
                i++;
            } else if (c == '"') {
                inQuotes = 0;
            } else {
                buffer[bufferLength++] = c;
            }
        } else if (c == '"' && bufferLength == 0) {
            inQuotes = 1;
        } else if (c == ',') {
            storeField(fields, &fieldCount, maxFields, buffer, bufferLength);
            bufferLength = 0;
        } else {
            buffer[bufferLength++] = c;
        }
    }
    storeField(fields, &fieldCount, maxFields, buffer, bufferLength);

    free(buffer);
    return fieldCount;
}
