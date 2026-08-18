/* record.h
 *
 * Defines the wildlife sighting record used throughout the dictionary,
 * and the operations for reading records out of the project's CSV
 * dataset one at a time. This module knows how to turn a dataset file
 * into a stream of Records, but nothing about how those Records end up
 * being stored (linked list, tree, etc.) - that is left to whichever
 * dictionary structure calls readNextRecord().
 */
#ifndef RECORD_H
#define RECORD_H

#include <stdio.h>

typedef struct {
    char *key;
    char *taxa;
    char *kingdom;
    char *phylum;
    char *taxonClass;
    char *order;
    char *family;
    char *genus;
    char *species;
    char *commonName;
    char *identifcat;
    char *dataResour;
    char *sightingDate;
    long double latitude;
    long double longitude;
    char *eziAdd;
} Record;

/* Opens filename for reading and discards its CSV header line, leaving
 * the file positioned at the first data row. Returns NULL (after
 * printing an error) if the file cannot be opened or is empty. */
FILE *openDatasetFile(const char *filename);

/* Reads and parses the next data row from file (as opened by
 * openDatasetFile()) into a freshly malloc'd Record. Returns NULL once
 * the end of the file is reached. The caller owns the returned Record
 * and must free it with freeRecord(). */
Record *readNextRecord(FILE *file);

/* Frees a Record and all of the strings it owns. */
void freeRecord(Record *record);

/* Writes one matched record to out in the "--> KEY: ... || ..." format
 * required for search results. */
void printRecordMatch(FILE *out, const Record *record);

#endif
