/* editdistance.c - see editdistance.h for the module's interface
 * documentation. Standard Wagner-Fischer dynamic programming solution, as
 * given in the assignment spec (reference: geeksforgeeks.org/edit-distance-in-c/).
 */

#include "editdistance.h"
#include "util.h"
#include <stdlib.h>

/* Returns the minimum of three integers. */
static int min(int a, int b, int c) {
    if (a < b) {
        return (a < c) ? a : c;
    }
    return (b < c) ? b : c;
}

int editDistance(char *str1, char *str2, int n, int m) {
    int **distance = safeMalloc((n + 1) * sizeof(int *));
    int i, j, result;

    for (i = 0; i <= n; i++) {
        distance[i] = safeMalloc((m + 1) * sizeof(int));
    }

    for (i = 0; i <= n; i++) {
        for (j = 0; j <= m; j++) {
            if (i == 0) {
                distance[i][j] = j;
            } else if (j == 0) {
                distance[i][j] = i;
            } else if (str1[i - 1] == str2[j - 1]) {
                distance[i][j] = distance[i - 1][j - 1];
            } else {
                distance[i][j] = 1 + min(distance[i - 1][j],
                                          distance[i][j - 1],
                                          distance[i - 1][j - 1]);
            }
        }
    }

    result = distance[n][m];

    for (i = 0; i <= n; i++) {
        free(distance[i]);
    }
    free(distance);

    return result;
}
