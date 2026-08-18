/* search.c - see search.h for the module's interface documentation. */

#include <stdlib.h>

#include "search.h"
#include "compare.h"
#include "util.h"

#define INITIAL_MATCH_CAPACITY 8

SearchResult *searchByKey(Dictionary *dictionary, char *key) {
    SearchResult *result = safeMalloc(sizeof(SearchResult));
    result->numMatches = 0;
    result->bitComparisons = 0;
    result->nodeComparisons = 0;
    result->stringComparisons = 0;

    int matchCapacity = INITIAL_MATCH_CAPACITY;
    result->matches = safeMalloc(matchCapacity * sizeof(Record *));

    DictNode *current = dictionary->head;
    while (current != NULL) {
        unsigned int bitsCompared = 0;
        int comparison =
            bitwiseStrcmp(current->record->key, key, &bitsCompared);

        result->nodeComparisons++;
        result->stringComparisons++;
        result->bitComparisons += bitsCompared;

        if (comparison == 0) {
            if (result->numMatches == matchCapacity) {
                matchCapacity *= 2;
                size_t newSize = matchCapacity * sizeof(Record *);
                result->matches = safeRealloc(result->matches, newSize);
            }
            result->matches[result->numMatches] = current->record;
            result->numMatches++;
        }

        current = current->next;
    }

    return result;
}

void freeSearchResult(SearchResult *result) {
    free(result->matches);
    free(result);
}
