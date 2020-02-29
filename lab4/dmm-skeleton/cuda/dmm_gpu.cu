/*
 *  dmm_gpu.cu -- Template for DMM GPU kernels
 *
 *  Copyright (C) 2019, Computing Systems Laboratory (CSLab)
 *  Copyright (C) 2019, Athena Elafrou
 */

#include "dmm.h"
#include <cublas_v2.h>

/*
 *  Naive kernel
 */
__global__ void dmm_gpu_naive(const value_t *A, const value_t *B, value_t *C,
                              const size_t M, const size_t N, const size_t K) {
    /*
   * FILLME: fill the code.
   */
    int i = blockDim.y * blockIdx.y + threadIdx.y;
    int j = blockDim.x * blockIdx.x + threadIdx.x;
    value_t _Cij = 0;
    for (size_t k = 0; k < K; ++k) {
        _Cij += A[K * i + k] * B[N * k + j];
    }

    C[N * i + j] = _Cij;
}

/*
 *  Coalesced memory acceses of A.
 */
__global__ void dmm_gpu_coalesced_A(const value_t *A, const value_t *B,
                                    value_t *C, const size_t M, const size_t N,
                                    const size_t K) {
    /*
   * FILLME: fill the code.
   */
    value_t _Cij = 0;

    int i = threadIdx.y;
    int j = threadIdx.x;

    for (int m = 0; m < K / BLOCK_SIZE; ++m) {
        __shared__ value_t As[BLOCK_SIZE][BLOCK_SIZE];

        As[i][j] = A[K * BLOCK_SIZE * blockIdx.y + BLOCK_SIZE * m + K * i + j];

        __syncthreads();

        for (int k = 0; k < BLOCK_SIZE; ++k)
            _Cij += As[i][k] * B[N * BLOCK_SIZE * m + BLOCK_SIZE * blockIdx.x + N * k + j];

        __syncthreads();
    }

    C[N * BLOCK_SIZE * blockIdx.y + BLOCK_SIZE * blockIdx.x + N * i + j] = _Cij;
}

/*
 *  Reduced memory accesses.
 */
__global__ void dmm_gpu_reduced_global(const value_t *A, const value_t *B, value_t *C,
                                       const size_t M, const size_t N, const size_t K) {
    /*
   * FILLME: fill the code.
   */
    value_t _Cij = 0;

    int i = threadIdx.y;
    int j = threadIdx.x;

    for (int m = 0; m < K / TILE_Y; ++m) {
        __shared__ value_t As[TILE_X][TILE_Y];
        __shared__ value_t Bs[TILE_Y][TILE_X];

        As[i][j] = A[K * (TILE_Y * blockIdx.y + i) + TILE_X * m + j];
        Bs[i][j] = B[N * (TILE_Y * m + i) + TILE_X * blockIdx.x + j];

        __syncthreads();

        for (int k = 0; k < TILE_Y; ++k)
            _Cij += As[i][k] * Bs[k][j];

        __syncthreads();
    }

    C[N * (TILE_Y * blockIdx.y + i) + TILE_X * blockIdx.x + j] = _Cij;
}

/*
 *  Use of cuBLAS
 */
void dmm_gpu_cublas(const value_t *A, const value_t *B, value_t *C,
                    const size_t M, const size_t N, const size_t K) {
    /*
   * FILLME: fill the code.
   */
    const float alpha = 1, beta = 0;
    cublasHandle_t handle; // move to main
    cublasCreate(&handle);
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, M, N, K, &alpha, A, M, B, K, &beta, C, M);
    cublasDestroy(handle);
}
