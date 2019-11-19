/*
  Tiled version of the Floyd-Warshall algorithm.
  command-line arguments: N, B
  N = size of graph
  B = size of tile
  works only when N is a multiple of B
*/
#include "tbb/parallel_for.h"
#include "tbb/parallel_invoke.h"
#include "tbb/task_scheduler_init.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

inline int min(int a, int b);
inline void FW(int **A, int K, int I, int J, int N);

int main(int argc, char **argv) {
    int **A;
    int i, j, k;
    struct timeval t1, t2;
    double time;
    int B = 64;
    int N = 1024;
    int nthreads;

    if (argc != 4) {
        fprintf(stdout, "Usage %s N B nthreads\n", argv[0]);
        exit(0);
    }

    N = atoi(argv[1]);
    B = atoi(argv[2]);
    nthreads = atoi(argv[3]);

    A = (int **)malloc(N * sizeof(int *));
    for (i = 0; i < N; i++)
        A[i] = (int *)malloc(N * sizeof(int));

    graph_init_random(A, -1, N, 128 * N);

    tbb::task_scheduler_init init(nthreads);

    gettimeofday(&t1, 0);

    for (k = 0; k < N; k += B) {
        FW(A, k, k, k, B);

        tbb::parallel_invoke(
            [&] {
                tbb::parallel_for(0, N, B, [&](int i) {
                    if (i != k)
                        FW(A, k, i, k, B);
                });
            },
            [&] {
                tbb::parallel_for(0, N, B, [&](int j) {
                    if (j != k)
                        FW(A, k, k, j, B);
                });
            });

        tbb::parallel_for(0, N, B, [&](int i) {
            for (int j = 0; j < N; j += B)
                if (i != k && j != k)
                    FW(A, k, i, j, B);
        });
    }
    gettimeofday(&t2, 0);

    time = (double)((t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec) / 1000000;
    printf("FW_TILED,%d,%d,%.4f\n", N, B, time);

    // for (i = 0; i < N; i++)
    //     for (j = 0; j < N; j++)
    //         fprintf(stdout, "%d\n", A[i][j]);

    return 0;
}

inline int min(int a, int b) {
    if (a <= b)
        return a;
    else
        return b;
}

inline void FW(int **A, int K, int I, int J, int N) {
    int i, j, k;

    for (k = K; k < K + N; k++)
        for (i = I; i < I + N; i++)
            for (j = J; j < J + N; j++)
                A[i][j] = min(A[i][j], A[i][k] + A[k][j]);
}