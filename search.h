/* search.h
 *
 * SearchResult: the outcome of a dictionary key lookup, independent of
 * whichever dictionary structure produced it. It holds the matched records
 * and the bit/node/string comparison counts used to reach them, so
 * different dictionary implementations (e.g. patriciaSearch.h's Patricia
 * tree lookup) can report results the same way.
 */
#ifndef SEARCH_H
#define SEARCH_H

#include "record.h"

typedef struct {
    Record **matches;
    int numMatches;
    unsigned long bitComparisons;
    unsigned long nodeComparisons;
    unsigned long stringComparisons;
} SearchResult;

/* Frees a SearchResult (but not the Records it points to). */
void freeSearchResult(SearchResult *result);

#endif
