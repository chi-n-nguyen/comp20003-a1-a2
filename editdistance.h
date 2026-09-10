/* editdistance.h
 *
 * Levenshtein edit distance between two nul-terminated strings, used by the
 * Patricia tree's spellchecker to rank candidate keys against a query that
 * had no exact match.
 */
#ifndef EDITDISTANCE_H
#define EDITDISTANCE_H

/* Returns the minimum number of single-character insertions, deletions, and
 * substitutions needed to turn str1 (length n) into str2 (length m). */
int editDistance(char *str1, char *str2, int n, int m);

#endif
