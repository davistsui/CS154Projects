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

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

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
    // Define constants
	// Each transpose function access at most 12 local variables
    int i, j; // row, col block index, accessing one block at a time
    int k, l; // row, col index inside each block
    int blocksize;
    // other constants
    int temp, diagonal;
	// specific to 64 by 64 case
    int t0, t1, t2, t3, t4, t5, t6, t7;

    // case for M = 32 and N = 32
    if ((M == 32) && (N == 32)) {
        blocksize = 8;
        for (i = 0; i < N; i += blocksize) {
            for (j = 0; j < M; j += blocksize) {
                for (k = i; k < i + blocksize; k++) {
                    for (l = j; l < j + blocksize; l++) {
                        // need to consider both off-diagonal and diagonal cases
                        if (k != l) 
                            B[l][k] = A[k][l];
                        else {
                            temp = A[k][l];
                            diagonal = k;
                        }
                    }
                    if (i == j) 
                        B[diagonal][diagonal] = temp;
                }
            }
        }
    }

    // case for M = 32 and N = 64
    if ((M == 32) && (N == 64)) {
        for (i = 0; i < N; i += 8) {
            for (j = 0; j < M; j += 4) {
                // needs to check for boundaries in inner loops
                for (k = i; (k < i + 8) && (k < N); k++) {
                    for (l = j; (l < j + 4) && (l < M); l++) {
                        B[l][k] = A[k][l];    
                    }
                }
            }
        }
    }

    // case for M = 64 and N = 64
    if ((M == 64) && (N == 64)) {
        // Here we still use 8 by 8 blocksize
        // but insde this block, we divide them up to four sub-blocks as shown 
        // | a | b |
        // | c | d |
        blocksize = 8;
        for (i = 0; i < N; i += blocksize) {
            for (j = 0; j < M; j += blocksize) {
				// For sub-block a transpose
				for (k = i; k < i + 4; k++) {
					for (l = j; l < j + 4; l++) {
						B[l][k] = A[k][l];
					}
				}
                // For sub-block b
                // Here we don't transpose it directory due to cache conflict
                // since we still want to utilize the advantange of cache block size
                // we store first part of sub-block b into local variables
	            t0 = A[i][j+4];
	            t1 = A[i][j+5];
	            t2 = A[i][j+6];
	            t3 = A[i][j+7];
	            t4 = A[i+1][j+4];
	            t5 = A[i+1][j+5];
	            t6 = A[i+1][j+6];
	            t7 = A[i+1][j+7];
				

				// For sub-block c transpose
				for (k = i + 4; k < i + 8; k++) {
					for (l = j; l < j + 4; l++) {
						B[l][k] = A[k][l];
					}
				}
         	   		
				// For sub-block d transpose
				for (k = i + 4; k < i + 8; k++) {
					for (l = j + 4; l < j + 8; l++) {
						B[l][k] = A[k][l];
					}
				}
				
				// For sub-block b second part
                // we do transpose here
	            B[j+4][i] = t0;
	            B[j+5][i] = t1;
	            B[j+6][i] = t2;
	            B[j+7][i] = t3;
	            B[j+4][i+1] = t4;
	            B[j+5][i+1] = t5;
	            B[j+6][i+1] = t6;
	            B[j+7][i+1] = t7;
		
                // transpose the second part of sub-block b
                // (since we only allowed to use up to 12 stack variables
				for (k = i + 2; k < i + 4; k++) {
					for (l = j + 4; l < j + 8; l++) {
						B[l][k] = A[k][l];
					}
				}
			}
		}
	}

    // case for M = 61 and N = 67
    if ((M == 61) && (N == 67)) {
        blocksize = 16;
        for (i = 0; i < N; i += blocksize) {
            for (j = 0; j < M; j+= blocksize) {
                for (k = i; (k < i + blocksize) && (k < N); k++) {
                    for (l = j; (l < j + blocksize) && (l < M); l++) {
                        B[l][k] = A[k][l];
                    }
                }
            }
        }
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
	
	//registerTransFunction(trans_col, trans_col_desc);

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

