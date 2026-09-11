/* dictionary.h
 *
 * A linked-list dictionary of wildlife Records, preserving insertion
 * order. Every operation takes an explicit Dictionary pointer so that
 * more than one dictionary can exist (and be searched) at once.
 */
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "record.h"
#include "search.h"

typedef struct DictNode {
    Record *record;
    struct DictNode *next;
} DictNode;

typedef struct {
    DictNode *head;
    DictNode *tail;
    int numRecords;
} Dictionary;

/* Creates an empty dictionary. */
Dictionary *createDictionary(void);

/* Appends record to the end of dictionary, preserving the order records
 * are inserted in. dictionary takes ownership of record. */
void insertRecord(Dictionary *dictionary, Record *record);

/* Searches dictionary for every record whose key matches key exactly,
 * comparing keys bit by bit and counting bit, node, and string
 * comparisons as it goes. Every node in dictionary is visited, since
 * matching records are not guaranteed to be unique or sorted. The
 * returned SearchResult's matches point directly at records owned by
 * dictionary; freeing the result does not free those records. */
SearchResult *searchDictionaryByKey(Dictionary *dictionary, char *key);

/* Frees dictionary, all of its nodes, and all of the records they hold. */
void freeDictionary(Dictionary *dictionary);

#endif
