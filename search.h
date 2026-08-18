/* search.h
 *
 * Key lookup over a Dictionary. A SearchResult holds the outcome of a
 * search independently of the dictionary's own structure: the matched
 * records and the bit/node/string comparison counts can all be read and
 * used without touching the dictionary's linked list again.
 */
#ifndef SEARCH_H
#define SEARCH_H

#include "dictionary.h"

typedef struct {
    Record **matches;
    int numMatches;
    unsigned long bitComparisons;
    unsigned long nodeComparisons;
    unsigned long stringComparisons;
} SearchResult;

/* Searches dictionary for every record whose key matches key exactly,
 * comparing keys bit by bit and counting bit, node, and string
 * comparisons as it goes. Every node in dictionary is visited, since
 * matching records are not guaranteed to be unique or sorted. The
 * returned SearchResult's matches point directly at records owned by
 * dictionary; freeing the result does not free those records. */
SearchResult *searchByKey(Dictionary *dictionary, char *key);

/* Frees a SearchResult (but not the Records it points to). */
void freeSearchResult(SearchResult *result);

#endif
