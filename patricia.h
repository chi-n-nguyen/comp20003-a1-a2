/* patricia.h
 *
 * A Patricia (radix-2, bitwise) trie dictionary of wildlife Records, keyed
 * on KEY's bits (via bit.h's getBit()/createStem()) rather than whole
 * characters. Every node stores only the bit-stem for its own edge (not an
 * accumulated root-to-node prefix), following the assignment's "Extra
 * Insertion Example". A node is either:
 *   - a leaf: records != NULL, branchA == branchB == NULL, holding every
 *     Record inserted under this exact key, in file order; or
 *   - internal: records == NULL, branchA and branchB both non-NULL.
 * This split is exhaustive because every key is nul-terminated: two
 * distinct keys can never share a full all-zero terminator byte while one
 * of them continues afterwards, so a node completing one key can never also
 * be a prefix of another.
 *
 * Every operation takes an explicit PatriciaTree pointer so more than one
 * tree can exist at once.
 */
#ifndef PATRICIA_H
#define PATRICIA_H

#include "record.h"

typedef struct RecordListNode {
    Record *record;
    struct RecordListNode *next;
} RecordListNode;

typedef struct PatriciaNode {
    char *stem;
    unsigned int stemBits;
    struct PatriciaNode *branchA;
    struct PatriciaNode *branchB;
    RecordListNode *records;
    RecordListNode *recordsTail;
} PatriciaNode;

typedef struct {
    PatriciaNode *root;
    int numRecords;
} PatriciaTree;

/* Creates an empty Patricia tree. */
PatriciaTree *createPatriciaTree(void);

/* Inserts record into tree, keyed on record->key. If an existing leaf
 * already holds the exact same key, record is appended to that leaf's
 * record list, preserving insertion order; otherwise the tree is split as
 * needed to make room for a new leaf. tree takes ownership of record. */
void insertPatriciaRecord(PatriciaTree *tree, Record *record);

/* Frees tree, all of its nodes and stems, and all of the records held at
 * its leaves. */
void freePatriciaTree(PatriciaTree *tree);

#endif
