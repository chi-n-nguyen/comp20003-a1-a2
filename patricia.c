/* patricia.c - see patricia.h for the module's interface documentation. */

#include <stdlib.h>
#include <string.h>

#include "patricia.h"
#include "bit.h"
#include "util.h"

static PatriciaNode *makeLeaf(char *key, unsigned int startBit,
                               unsigned int numBits, Record *record) {
    PatriciaNode *leaf = safeMalloc(sizeof(PatriciaNode));
    leaf->stem = createStem(key, startBit, numBits);
    leaf->stemBits = numBits;
    leaf->branchA = NULL;
    leaf->branchB = NULL;

    RecordListNode *node = safeMalloc(sizeof(RecordListNode));
    node->record = record;
    node->next = NULL;
    leaf->records = node;
    leaf->recordsTail = node;

    return leaf;
}

/* Appends record to the end of leaf's record list, preserving the order
 * duplicate keys were inserted in. */
static void appendRecord(PatriciaNode *leaf, Record *record) {
    RecordListNode *node = safeMalloc(sizeof(RecordListNode));
    node->record = record;
    node->next = NULL;
    leaf->recordsTail->next = node;
    leaf->recordsTail = node;
}

/* Inserts record (whose key is keyBits bits long, including its terminating
 * nul byte) into the subtree rooted at *nodePtr, whose own edge begins at
 * bit depth of the key. May replace *nodePtr with a new split node. */
static void insertIntoSubtree(PatriciaNode **nodePtr, char *key,
                               unsigned int keyBits, unsigned int depth,
                               Record *record) {
    PatriciaNode *node = *nodePtr;
    unsigned int matchLen = 0;

    while (matchLen < node->stemBits &&
           getBit(key, depth + matchLen) == getBit(node->stem, matchLen)) {
        matchLen++;
    }

    if (matchLen == node->stemBits) {
        /* The whole stem matched. */
        if (node->records != NULL) {
            /* A leaf with this exact key already exists. */
            appendRecord(node, record);
            return;
        }

        /* An internal node: consume the branch-selector bit and recurse. */
        unsigned int nextDepth = depth + node->stemBits;
        PatriciaNode **child = getBit(key, nextDepth) == 0 ? &node->branchA
                                                            : &node->branchB;
        insertIntoSubtree(child, key, keyBits, nextDepth + 1, record);
        return;
    }

    /* The key diverges from this node partway through its stem: split the
     * node at matchLen. The differing bit itself (at local index matchLen)
     * is not stored in either child's stem - it is implied by which branch
     * each child hangs off. */
    int oldBit = getBit(node->stem, matchLen);

    PatriciaNode *oldSuffix = safeMalloc(sizeof(PatriciaNode));
    oldSuffix->stem =
        createStem(node->stem, matchLen + 1, node->stemBits - matchLen - 1);
    oldSuffix->stemBits = node->stemBits - matchLen - 1;
    oldSuffix->branchA = node->branchA;
    oldSuffix->branchB = node->branchB;
    oldSuffix->records = node->records;
    oldSuffix->recordsTail = node->recordsTail;

    unsigned int newLeafStart = depth + matchLen + 1;
    PatriciaNode *newLeaf =
        makeLeaf(key, newLeafStart, keyBits - newLeafStart, record);

    PatriciaNode *split = safeMalloc(sizeof(PatriciaNode));
    split->stem = createStem(node->stem, 0, matchLen);
    split->stemBits = matchLen;
    split->records = NULL;
    split->recordsTail = NULL;
    if (oldBit == 0) {
        split->branchA = oldSuffix;
        split->branchB = newLeaf;
    } else {
        split->branchA = newLeaf;
        split->branchB = oldSuffix;
    }

    free(node->stem);
    free(node);

    *nodePtr = split;
}

PatriciaTree *createPatriciaTree(void) {
    PatriciaTree *tree = safeMalloc(sizeof(PatriciaTree));
    tree->root = NULL;
    tree->numRecords = 0;
    return tree;
}

void insertPatriciaRecord(PatriciaTree *tree, Record *record) {
    unsigned int keyBits = (strlen(record->key) + 1) * BITS_PER_BYTE;

    if (tree->root == NULL) {
        tree->root = makeLeaf(record->key, 0, keyBits, record);
    } else {
        insertIntoSubtree(&tree->root, record->key, keyBits, 0, record);
    }
    tree->numRecords++;
}

static void freeNode(PatriciaNode *node) {
    if (node == NULL) {
        return;
    }

    freeNode(node->branchA);
    freeNode(node->branchB);
    free(node->stem);

    RecordListNode *current = node->records;
    while (current != NULL) {
        RecordListNode *next = current->next;
        freeRecord(current->record);
        free(current);
        current = next;
    }

    free(node);
}

void freePatriciaTree(PatriciaTree *tree) {
    freeNode(tree->root);
    free(tree);
}
