/* search.c - see search.h for the module's interface documentation. */

#include <stdlib.h>

#include "search.h"

void freeSearchResult(SearchResult *result) {
    free(result->matches);
    free(result);
}
