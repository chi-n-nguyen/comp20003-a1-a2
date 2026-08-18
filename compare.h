/* compare.h
 *
 * A strcmp()-like comparison between two nul-terminated strings performed
 * at the level of individual bits (built on top of getBit() from bit.h),
 * which also reports how many bit accesses were needed to reach a
 * verdict. This gives the dictionary's search operation the fine-grained
 * comparison counts described in the assignment's bit comparison
 * material.
 */
#ifndef COMPARE_H
#define COMPARE_H

/* Compares s1 and s2 bit by bit, from the most significant bit of the
 * first character onward, including the terminating nul byte. Returns a
 * negative, zero, or positive value if s1 is respectively less than,
 * equal to, or greater than s2 (mirroring strcmp()).
 *
 * *bitsCompared is set to the number of bits examined: this counts up to
 * and including the first mismatching bit, or, if the strings are
 * identical, every bit through to the end of the terminating nul byte. */
int bitwiseStrcmp(char *s1, char *s2, unsigned int *bitsCompared);

#endif
