/* dict1.c
 *
 * Created by: <Your Name> (<your student email>)
 *
 * Stage 1 of the wildlife dictionary: builds a linked-list dictionary of
 * wildlife sighting records from a CSV dataset, then answers KEY lookup
 * queries read from stdin. Matching records for each query are written
 * to the given output file (or NOTFOUND if there are none), and a
 * one-line summary - including bit, node, and string comparison counts
 * for that query - is written to stdout.
 *
 * To run:
 *   make dict1
 *   ./dict1 1 <dataset.csv> <output.txt> < queries.in
 */

/* Needed for getline(), which is POSIX.1-2008 rather than C99. */
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>

#include "record.h"
#include "dictionary.h"
#include "search.h"
#include "util.h"

#define STAGE_LOOKUP 1
#define EXPECTED_ARGC 4

static Dictionary *buildDictionary(const char *csvFilename);
static void runQueries(Dictionary *dictionary, FILE *outputFile);

int main(int argc, char **argv) {
    if (argc != EXPECTED_ARGC) {
        fprintf(stderr, "Usage: %s <stage> <csv-file> <output-file>\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    int stage = atoi(argv[1]);
    if (stage != STAGE_LOOKUP) {
        fprintf(stderr, "Error: dict1 only supports stage %d (lookup).\n",
                STAGE_LOOKUP);
        exit(EXIT_FAILURE);
    }

    Dictionary *dictionary = buildDictionary(argv[2]);

    FILE *outputFile = fopen(argv[3], "w");
    if (outputFile == NULL) {
        fprintf(stderr, "Error: could not open output file '%s'.\n", argv[3]);
        freeDictionary(dictionary);
        exit(EXIT_FAILURE);
    }

    runQueries(dictionary, outputFile);

    fclose(outputFile);
    freeDictionary(dictionary);

    return 0;
}

/* Reads every record out of the CSV file at csvFilename, in file order,
 * into a freshly built Dictionary. */
static Dictionary *buildDictionary(const char *csvFilename) {
    FILE *csvFile = openDatasetFile(csvFilename);
    if (csvFile == NULL) {
        exit(EXIT_FAILURE);
    }

    Dictionary *dictionary = createDictionary();

    Record *record;
    while ((record = readNextRecord(csvFile)) != NULL) {
        insertRecord(dictionary, record);
    }

    fclose(csvFile);
    return dictionary;
}

/* Reads KEY queries from stdin, one per line, until end of file. Each
 * query's matches (or NOTFOUND) are written to outputFile, and a
 * comparison-count summary is written to stdout. */
static void runQueries(Dictionary *dictionary, FILE *outputFile) {
    char *queryLine = NULL;
    size_t queryCapacity = 0;

    while (getline(&queryLine, &queryCapacity, stdin) != -1) {
        stripLineEnding(queryLine);

        SearchResult *result = searchByKey(dictionary, queryLine);

        fprintf(outputFile, "%s\n", queryLine);
        if (result->numMatches == 0) {
            fprintf(outputFile, "NOTFOUND\n");
        } else {
            int i;
            for (i = 0; i < result->numMatches; i++) {
                printRecordMatch(outputFile, result->matches[i]);
            }
        }

        printf("%s --> %d records found - comparisons: b%lu n%lu s%lu\n",
               queryLine, result->numMatches, result->bitComparisons,
               result->nodeComparisons, result->stringComparisons);

        freeSearchResult(result);
    }

    free(queryLine);
}
