/* dictionary.c - see dictionary.h for the module's interface documentation. */

#include <stdlib.h>

#include "dictionary.h"
#include "compare.h"
#include "util.h"

#define INITIAL_MATCH_CAPACITY 8

Dictionary *createDictionary(void) {
    Dictionary *dictionary = safeMalloc(sizeof(Dictionary));
    dictionary->head = NULL;
    dictionary->tail = NULL;
    dictionary->numRecords = 0;
    return dictionary;
}

void insertRecord(Dictionary *dictionary, Record *record) {
    DictNode *node = safeMalloc(sizeof(DictNode));
    node->record = record;
    node->next = NULL;

    if (dictionary->tail == NULL) {
        dictionary->head = node;
    } else {
        dictionary->tail->next = node;
    }
    dictionary->tail = node;
    dictionary->numRecords++;
}

SearchResult *searchDictionaryByKey(Dictionary *dictionary, char *key) {
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

void freeDictionary(Dictionary *dictionary) {
    DictNode *current = dictionary->head;

    while (current != NULL) {
        DictNode *next = current->next;
        freeRecord(current->record);
        free(current);
        current = next;
    }

    free(dictionary);
}
