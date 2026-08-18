#include <stdlib.h>

#include "dictionary.h"
#include "util.h"

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
