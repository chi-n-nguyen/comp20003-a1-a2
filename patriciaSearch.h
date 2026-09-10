/* patriciaSearch.h
 *
 * Key lookup over a PatriciaTree. Reuses search.h's dictionary-agnostic
 * SearchResult type (and its freeSearchResult()) so results are reported
 * the same way regardless of the underlying dictionary structure.
 *
 * Unlike the linked list, a Patricia tree lookup may not find an exact
 * match for key. When it doesn't (and the tree is non-empty), the point
 * where key's bits first diverge from the tree identifies a subtree of
 * "close" candidate keys; the candidate with the lowest edit distance to
 * key is returned instead, as a spelling recommendation.
 */
#ifndef PATRICIA_SEARCH_H
#define PATRICIA_SEARCH_H

#include "patricia.h"
#include "search.h"

/* Searches tree for key. If an exact match exists, every record stored
 * under that key is returned (in file order). Otherwise, if tree is
 * non-empty, the records for the closest matching key (by edit distance,
 * ties broken in favour of the alphabetically first key) are returned
 * instead. If tree is empty, the result has zero matches.
 *
 * bitComparisons and nodeComparisons count every bit examined and node
 * visited while descending the tree to find the exact match or mismatch
 * point; stringComparisons counts the single whole-key resolution (to an
 * exact match, or to a mismatch point) that determined the result. Edit
 * distance calculations used to rank spellcheck candidates are not counted,
 * since that cost applies equally regardless of the underlying dictionary
 * structure. */
SearchResult *searchPatriciaByKey(PatriciaTree *tree, char *key);

#endif
