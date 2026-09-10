/* patriciaSearch.c - see patriciaSearch.h for the module's interface
 * documentation. */

#include <string.h>

#include "patriciaSearch.h"
#include "bit.h"
#include "editdistance.h"
#include "util.h"

#define INITIAL_MATCH_CAPACITY 8

/* Appends every record held at leaf to result->matches, in file order,
 * growing the matches array via *matchCapacity as needed. */
static void copyLeafRecords(SearchResult *result, int *matchCapacity,
                             PatriciaNode *leaf) {
    RecordListNode *current = leaf->records;

    while (current != NULL) {
        if (result->numMatches == *matchCapacity) {
            *matchCapacity *= 2;
            size_t newSize = (*matchCapacity) * sizeof(Record *);
            result->matches = safeRealloc(result->matches, newSize);
        }
        result->matches[result->numMatches] = current->record;
        result->numMatches++;
        current = current->next;
    }
}

/* Searches every leaf in the subtree rooted at node for the one whose key
 * has the lowest edit distance to query, breaking ties in favour of the
 * first leaf found. Visiting branchA (the "0" branch) before branchB (the
 * "1" branch) visits leaves in ascending bit order - the same order as
 * alphabetical order for this dataset's keys - so the first leaf found at
 * the lowest distance is always the alphabetically earliest one, matching
 * the assignment's tie-breaking rule with no extra bookkeeping. */
static void findBestCandidate(PatriciaNode *node, char *query,
                               PatriciaNode **bestLeaf, int *bestDistance) {
    if (node == NULL) {
        return;
    }

    if (node->records != NULL) {
        char *candidateKey = node->records->record->key;
        int distance = editDistance(query, candidateKey, (int)strlen(query),
                                     (int)strlen(candidateKey));
        if (*bestLeaf == NULL || distance < *bestDistance) {
            *bestDistance = distance;
            *bestLeaf = node;
        }
        return;
    }

    findBestCandidate(node->branchA, query, bestLeaf, bestDistance);
    findBestCandidate(node->branchB, query, bestLeaf, bestDistance);
}

SearchResult *searchPatriciaByKey(PatriciaTree *tree, char *key) {
    SearchResult *result = safeMalloc(sizeof(SearchResult));
    result->numMatches = 0;
    result->bitComparisons = 0;
    result->nodeComparisons = 0;
    result->stringComparisons = 0;

    int matchCapacity = INITIAL_MATCH_CAPACITY;
    result->matches = safeMalloc(matchCapacity * sizeof(Record *));

    if (tree->root == NULL) {
        return result;
    }

    unsigned int queryBits = (unsigned int)(strlen(key) + 1) * BITS_PER_BYTE;
    PatriciaNode *node = tree->root;
    PatriciaNode *candidate = NULL;
    unsigned int depth = 0;
    int exactMatch = 0;

    while (node != NULL) {
        result->nodeComparisons++;

        unsigned int matchLen = 0;
        int mismatch = 0;
        while (matchLen < node->stemBits) {
            if (depth + matchLen >= queryBits) {
                /* key has no more bits left to compare, but this node's
                 * stem does: they cannot match. Guards against reading
                 * past key's own buffer. */
                mismatch = 1;
                break;
            }
            result->bitComparisons++;
            if (getBit(key, depth + matchLen) != getBit(node->stem, matchLen)) {
                mismatch = 1;
                break;
            }
            matchLen++;
        }

        if (mismatch) {
            candidate = node;
            break;
        }

        depth += node->stemBits;

        if (node->records != NULL) {
            /* Every bit of the stem matched and this is a leaf: key is an
             * exact match. */
            candidate = node;
            exactMatch = 1;
            break;
        }

        /* Whole stem matched at an internal node: consume the
         * branch-selector bit and descend. */
        result->bitComparisons++;
        int branchBit = getBit(key, depth);
        depth++;
        node = (branchBit == 0) ? node->branchA : node->branchB;
    }

    if (candidate == NULL) {
        return result;
    }

    result->stringComparisons = 1;

    if (exactMatch) {
        copyLeafRecords(result, &matchCapacity, candidate);
    } else {
        PatriciaNode *bestLeaf = NULL;
        int bestDistance = 0;
        findBestCandidate(candidate, key, &bestLeaf, &bestDistance);
        copyLeafRecords(result, &matchCapacity, bestLeaf);
    }

    return result;
}
