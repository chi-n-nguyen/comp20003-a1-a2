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

/* Allocates and returns a new buffer holding the numBits bits of oldKey
 * starting at startBit (using the same left-to-right bit order as getBit()).
 * The result is packed 8 bits per byte, with any leftover bits in the final
 * byte padded with 0, so it is exactly (numBits + 7) / 8 bytes long. It is
 * not a C string: it is not nul-terminated by this function and may not be
 * safe to print or measure with strlen(). The caller owns the returned
 * buffer and must free() it. */
char *createStem(char *oldKey, unsigned int startBit, unsigned int numBits);

#endif
