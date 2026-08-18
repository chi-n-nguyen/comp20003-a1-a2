/* bit.h
 *
 * Bit-level access into a nul-terminated byte string, as provided in the
 * assignment's "Bit Comparisons" material. Bits are indexed from 0 at the
 * most significant bit of the first character, so that comparing keys bit
 * by bit visits them in the same left-to-right order as comparing them
 * character by character.
 */
#ifndef BIT_H
#define BIT_H

/* Number of bits in a single character. */
#define BITS_PER_BYTE 8

/* Gets the bit at bitIndex from the string s, counting from the most
 * significant bit of s[0] as bit 0. */
int getBit(char *s, unsigned int bitIndex);

#endif
