/* dictionary.h
 *
 * A linked-list dictionary of wildlife Records, preserving insertion
 * order. Every operation takes an explicit Dictionary pointer so that
 * more than one dictionary can exist (and be searched) at once.
 */
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include "record.h"

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

/* Frees dictionary, all of its nodes, and all of the records they hold. */
void freeDictionary(Dictionary *dictionary);

#endif
