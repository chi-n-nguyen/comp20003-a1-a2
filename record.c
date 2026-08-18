#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "record.h"
#include "csv.h"
#include "util.h"

/* Column order in the dataset's CSV header: KEY, Taxa, Kingdom, Phylum,
 * Class, Order, Family, Genus, Species, Common_Name, Identificat,
 * Data_Resour, Sighting_Da, Latitude, Longitude, EZI_ADD. */
enum {
    FIELD_KEY = 0,
    FIELD_TAXA,
    FIELD_KINGDOM,
    FIELD_PHYLUM,
    FIELD_CLASS,
    FIELD_ORDER,
    FIELD_FAMILY,
    FIELD_GENUS,
    FIELD_SPECIES,
    FIELD_COMMON_NAME,
    FIELD_IDENTIFCAT,
    FIELD_DATA_RESOUR,
    FIELD_SIGHTING_DATE,
    FIELD_LATITUDE,
    FIELD_LONGITUDE,
    FIELD_EZI_ADD,
    NUM_FIELDS
};

FILE *openDatasetFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: could not open dataset file '%s'.\n", filename);
        return NULL;
    }

    char *headerLine = NULL;
    size_t headerCapacity = 0;
    if (getline(&headerLine, &headerCapacity, file) == -1) {
        fprintf(stderr, "Error: dataset file '%s' is empty.\n", filename);
        free(headerLine);
        fclose(file);
        return NULL;
    }
    free(headerLine);

    return file;
}

Record *readNextRecord(FILE *file) {
    char *line = NULL;
    size_t lineCapacity = 0;

    if (getline(&line, &lineCapacity, file) == -1) {
        free(line);
        return NULL;
    }
    stripLineEnding(line);

    char *fields[NUM_FIELDS];
    int numFields = csvSplitLine(line, fields, NUM_FIELDS);
    free(line);

    if (numFields != NUM_FIELDS) {
        fprintf(stderr, "Error: expected %d CSV fields but found %d.\n",
                NUM_FIELDS, numFields);
        exit(EXIT_FAILURE);
    }

    Record *record = safeMalloc(sizeof(Record));
    record->key = fields[FIELD_KEY];
    record->taxa = fields[FIELD_TAXA];
    record->kingdom = fields[FIELD_KINGDOM];
    record->phylum = fields[FIELD_PHYLUM];
    record->taxonClass = fields[FIELD_CLASS];
    record->order = fields[FIELD_ORDER];
    record->family = fields[FIELD_FAMILY];
    record->genus = fields[FIELD_GENUS];
    record->species = fields[FIELD_SPECIES];
    record->commonName = fields[FIELD_COMMON_NAME];
    record->identifcat = fields[FIELD_IDENTIFCAT];
    record->dataResour = fields[FIELD_DATA_RESOUR];
    record->sightingDate = fields[FIELD_SIGHTING_DATE];
    record->eziAdd = fields[FIELD_EZI_ADD];

    record->latitude = strtold(fields[FIELD_LATITUDE], NULL);
    record->longitude = strtold(fields[FIELD_LONGITUDE], NULL);
    free(fields[FIELD_LATITUDE]);
    free(fields[FIELD_LONGITUDE]);

    return record;
}

void freeRecord(Record *record) {
    if (record == NULL) {
        return;
    }

    free(record->key);
    free(record->taxa);
    free(record->kingdom);
    free(record->phylum);
    free(record->taxonClass);
    free(record->order);
    free(record->family);
    free(record->genus);
    free(record->species);
    free(record->commonName);
    free(record->identifcat);
    free(record->dataResour);
    free(record->sightingDate);
    free(record->eziAdd);
    free(record);
}

/* Field labels intentionally mirror the assignment's own sample output
 * rather than the dataset's literal CSV header spellings/case (e.g.
 * "Identifcat" not "Identificat", lowercase "latitude"/"longitude"), so
 * that output matches the provided test cases exactly. */
void printRecordMatch(FILE *out, const Record *record) {
    fprintf(out,
            "--> KEY: %s || Taxa: %s || Kingdom: %s || Phylum: %s || "
            "Class: %s || Order: %s || Family: %s || Genus: %s || "
            "Species: %s || Common_Name: %s || Identifcat: %s || "
            "Data_Resour: %s || Sighting_Da: %s || latitude: %.5Lf || "
            "longitude: %.5Lf || EZI_ADD: %s || \n",
            record->key, record->taxa, record->kingdom, record->phylum,
            record->taxonClass, record->order, record->family, record->genus,
            record->species, record->commonName, record->identifcat,
            record->dataResour, record->sightingDate, record->latitude,
            record->longitude, record->eziAdd);
}
