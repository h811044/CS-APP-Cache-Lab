/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

#define BLOCKING_SIZE_32_32 8   /* must be a factor of 32 */
#define STRIDE_32_32 8  /* must be a factor of BLOCKING_SIZE_32_32 */
#define BLOCKING_SIZE_64_64_M 4 /* must be a factor of 64 */
#define BLOCKING_SIZE_64_64_N 8 /* must be a factor of 64 */
#define STRIDE_64_64 4  /* must be a factor of BLOCKING_SIZE_64_64_M */
#define BLOCKING_SIZE_61_67 23
#define BLOCKING_SIZE_DEF 1

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

void transpose_32_32(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, l;
    int tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7; /* stride 8 */
    M = M / BLOCKING_SIZE_32_32;
    N = N / BLOCKING_SIZE_32_32;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            for (k = 0; k < BLOCKING_SIZE_32_32; k++) {
                for (l = 0; l < BLOCKING_SIZE_32_32; l+=STRIDE_32_32) {
                    tmp0 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l];
                    tmp1 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+1];
                    tmp2 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+2];
                    tmp3 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+3];
                    tmp4 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+4];
                    tmp5 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+5];
                    tmp6 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+6];
                    tmp7 = A[i*BLOCKING_SIZE_32_32+k][j*BLOCKING_SIZE_32_32+l+7];
                    B[j*BLOCKING_SIZE_32_32+l][i*BLOCKING_SIZE_32_32+k] = tmp0;
                    B[j*BLOCKING_SIZE_32_32+l+1][i*BLOCKING_SIZE_32_32+k] = tmp1;
                    B[j*BLOCKING_SIZE_32_32+l+2][i*BLOCKING_SIZE_32_32+k] = tmp2;
                    B[j*BLOCKING_SIZE_32_32+l+3][i*BLOCKING_SIZE_32_32+k] = tmp3;
                    B[j*BLOCKING_SIZE_32_32+l+4][i*BLOCKING_SIZE_32_32+k] = tmp4;
                    B[j*BLOCKING_SIZE_32_32+l+5][i*BLOCKING_SIZE_32_32+k] = tmp5;
                    B[j*BLOCKING_SIZE_32_32+l+6][i*BLOCKING_SIZE_32_32+k] = tmp6;
                    B[j*BLOCKING_SIZE_32_32+l+7][i*BLOCKING_SIZE_32_32+k] = tmp7;
                }
            }
        }
    }
}

void transpose_64_64(int M, int N, int A[N][M], int B[M][N])
{
    /*
     *  4   4   4   4   4   4   4   4   5   5   5   5   ...
     *  12  12  12  12  12  12  12  12  13  13  13  13  ...
     *  20  20  20  20  20  20  20  20  21  21  21  21  ...
     *  28  28  28  28  28  28  28  28  29  29  29  29  ...
     *  4   4   4   4   ...
     *  12  ...
     *  ...
     */

    int i, j, k, l;
    int tmp0, tmp1, tmp2, tmp3; /* stride 4 */
    int tmp4, tmp5, tmp6, tmp7; /* stride 4 */
    M = M / BLOCKING_SIZE_64_64_M;
    N = N / BLOCKING_SIZE_64_64_N;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            for (k = 0; k < BLOCKING_SIZE_64_64_N/2; k++) {
                for (l = 0; l < BLOCKING_SIZE_64_64_M; l+=STRIDE_64_64) {
                    tmp0 = A[i*BLOCKING_SIZE_64_64_N+k][j*BLOCKING_SIZE_64_64_M+l];
                    tmp1 = A[i*BLOCKING_SIZE_64_64_N+k][j*BLOCKING_SIZE_64_64_M+l+1];
                    tmp2 = A[i*BLOCKING_SIZE_64_64_N+k][j*BLOCKING_SIZE_64_64_M+l+2];
                    tmp3 = A[i*BLOCKING_SIZE_64_64_N+k][j*BLOCKING_SIZE_64_64_M+l+3];
                    tmp4 = A[i*BLOCKING_SIZE_64_64_N+k+4][j*BLOCKING_SIZE_64_64_M+l];
                    tmp5 = A[i*BLOCKING_SIZE_64_64_N+k+4][j*BLOCKING_SIZE_64_64_M+l+1];
                    tmp6 = A[i*BLOCKING_SIZE_64_64_N+k+4][j*BLOCKING_SIZE_64_64_M+l+2];
                    tmp7 = A[i*BLOCKING_SIZE_64_64_N+k+4][j*BLOCKING_SIZE_64_64_M+l+3];
                    B[j*BLOCKING_SIZE_64_64_M+l][i*BLOCKING_SIZE_64_64_N+k] = tmp0;
                    B[j*BLOCKING_SIZE_64_64_M+l+1][i*BLOCKING_SIZE_64_64_N+k] = tmp1;
                    B[j*BLOCKING_SIZE_64_64_M+l+2][i*BLOCKING_SIZE_64_64_N+k] = tmp2;
                    B[j*BLOCKING_SIZE_64_64_M+l+3][i*BLOCKING_SIZE_64_64_N+k] = tmp3;
                    B[j*BLOCKING_SIZE_64_64_M+l][i*BLOCKING_SIZE_64_64_N+k+4] = tmp4;
                    B[j*BLOCKING_SIZE_64_64_M+l+1][i*BLOCKING_SIZE_64_64_N+k+4] = tmp5;
                    B[j*BLOCKING_SIZE_64_64_M+l+2][i*BLOCKING_SIZE_64_64_N+k+4] = tmp6;
                    B[j*BLOCKING_SIZE_64_64_M+l+3][i*BLOCKING_SIZE_64_64_N+k+4] = tmp7;
                }
            }
        }
    }
}

void transpose_default(int M, int N, int A[N][M], int B[M][N], int blocking_size)
{
    int i, j, k, l;
    int tmp;    /* stride 1 */
    int m, n;
    m = M / blocking_size + 1;
    n = N / blocking_size + 1;

    for (i = 0; i < n; i++) {
        for (j = 0; j < m; j++) {
            for (k = 0; k < blocking_size; k++) {
                for (l = 0; l < blocking_size; l++) {
                    if (i*blocking_size+k >= N || j*blocking_size+l >= M) {
                        continue;
                    }
                    tmp = A[i*blocking_size+k][j*blocking_size+l];
                    B[j*blocking_size+l][i*blocking_size+k] = tmp;
                }
            }
        }
    }
}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if (M == 32 && N == 32) {
        /* TEST_TRANS_RESULTS=1:287 */
        transpose_32_32(M, N, A, B);
    } else if (M == 64 && N == 64) {
        /* TEST_TRANS_RESULTS=1:1587 */
        transpose_64_64(M, N, A, B);
    } else if (M == 61 && N == 67) {
        /* TEST_TRANS_RESULTS=1:1928 */
        transpose_default(M, N, A, B, BLOCKING_SIZE_61_67);
    } else {
        transpose_default(M, N, A, B, BLOCKING_SIZE_DEF);
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

