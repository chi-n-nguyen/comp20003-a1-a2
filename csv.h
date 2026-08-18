/* csv.h
 *
 * A small, domain-independent CSV line splitter. It knows nothing about
 * wildlife records; it just turns one line of CSV text into an array of
 * field strings, so it can be reused for any comma-separated data.
 */
#ifndef CSV_H
#define CSV_H

/* Splits line on unquoted commas into at most maxFields fields, following
 * the CSV quoting rules given in the assignment: a field that contains a
 * comma is wrapped in double quotes, and a doubled double quote inside a
 * quoted field represents one literal double-quote character. The
 * wrapping quotes themselves are not included in the returned field.
 *
 * Each fields[i] is set to a freshly malloc'd, exact-length,
 * nul-terminated string; the caller takes ownership and must free() each
 * one. Returns the number of fields found. line must not contain '\n' or
 * '\r' characters (strip these before calling). */
int csvSplitLine(const char *line, char *fields[], int maxFields);

#endif
