/* compare.c - see compare.h for the module's interface documentation. */

#include "compare.h"
#include "bit.h"

int bitwiseStrcmp(char *s1, char *s2, unsigned int *bitsCompared) {
    unsigned int bitIndex = 0;

    *bitsCompared = 0;
    while (1) {
        int bit1 = getBit(s1, bitIndex);
        int bit2 = getBit(s2, bitIndex);
        (*bitsCompared)++;

        if (bit1 != bit2) {
            return (bit1 < bit2) ? -1 : 1;
        }

        /* Once a whole byte has matched with no mismatch, check whether it
         * was the nul terminator: since every printable character byte is
         * non-zero, a mismatch is always found within the byte where one
         * string ends and the other continues, so it is never necessary
         * (or safe) to read past a string's terminator. */
        if (bitIndex % BITS_PER_BYTE == BITS_PER_BYTE - 1) {
            unsigned int byteIndex = bitIndex / BITS_PER_BYTE;
            if (s1[byteIndex] == '\0') {
                return 0;
            }
        }

        bitIndex++;
    }
}
