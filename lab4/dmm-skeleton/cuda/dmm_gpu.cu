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
    for (int i = blockIdx.y * blockDim.y + threadIdx.y; i < M; i += blockDim.y * gridDim.y)
        for (int j = blockIdx.x * blockDim.x + threadIdx.x; j < N; j += blockDim.x * gridDim.x) {
            value_t _Cij = 0;

            for (int k = 0; k < K; ++k)
                _Cij += A[i * K + k] * B[k * N + j];

            C[i * N + j] = _Cij;
        }
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
    for (int i = blockIdx.y * blockDim.y + threadIdx.y; i < M; i += blockDim.y * gridDim.y)
        for (int j = blockIdx.x * blockDim.x + threadIdx.x; j < N; j += blockDim.x * gridDim.x) {
            value_t _Cij = 0;

            for (int m = 0; m < K; m += TILE_X) {
                __shared__ value_t As[THREAD_BLOCK_Y][TILE_X];

                for (int n = threadIdx.x; n < TILE_X; n += blockDim.x)
                    As[threadIdx.y][n] = A[i * K + n + m];

                __syncthreads();

                for (int k = 0; k < TILE_X; ++k)
                    _Cij += As[threadIdx.y][k] * B[(k + m) * N + j];

                __syncthreads();
            }

            C[i * N + j] = _Cij;
        }
}

/*
 *  Reduced memory accesses.
 */
__global__ void dmm_gpu_reduced_global(const value_t *A, const value_t *B, value_t *C,
                                       const size_t M, const size_t N, const size_t K) {
    /*
   * FILLME: fill the code.
   */
    for (int i = blockIdx.y * blockDim.y + threadIdx.y; i < M; i += blockDim.y * gridDim.y)
        for (int j = blockIdx.x * blockDim.x + threadIdx.x; j < N; j += blockDim.x * gridDim.x) {
            value_t _Cij = 0;

            for (int m = 0; m < K; m += TILE_X) {
                __shared__ value_t As[THREAD_BLOCK_Y][TILE_X];
                __shared__ value_t Bs[TILE_X][THREAD_BLOCK_X];

                for (int n = threadIdx.x; n < TILE_X; n += blockDim.x)
                    As[threadIdx.y][n] = A[i * K + n + m];
                for (int n = threadIdx.y; n < TILE_X; n += blockDim.y)
                    Bs[n][threadIdx.x] = B[(n + m) * N + j];

                __syncthreads();

                for (int k = 0; k < TILE_X; ++k)
                    _Cij += As[threadIdx.y][k] * Bs[k][threadIdx.x];

                __syncthreads();
            }

            C[i * N + j] = _Cij;
        }
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

    cublasHandle_t handle;
    cublasCreate(&handle);
    cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, M, N, K, &alpha, A, M, B, K, &beta, C, M);
    cublasDestroy(handle);
}
