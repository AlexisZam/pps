#include "util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

void graph_init_random(int **adjm, int seed, int n, int m) {
    int i, j;

    srand48(seed);
    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            adjm[i][j] = abs(((int)lrand48()) % 1048576);

    for (i = 0; i < n; i++)
        adjm[i][i] = 0;
}
