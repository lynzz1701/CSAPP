/***********************************************************************

 * Name: Yining Liu
 * ID: 516030910125
 ***********************************************************************/
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
    int i,j,m,n;
    int tmp[8];

    //32*32 = 4*(8*8), simply use tmps to avoid conflicts between A & B 
    if(M==32&&N==32){ 
        for(i=0;i<N;i+=8){
            for(j=0;j<M;j+=8){
                for(n=i;n<i+8;n++){
                    for(m=0;m<8;m++){
                        tmp[m] = A[n][j+m];
                    }
                    for(m=0;m<8;m++){
                        B[j+m][n] = tmp[m];
                    }
                }
            }
        }
    }

    //64*64: most complicated among 3 cases.
    if(M==64&&N==64){ 
        for(i=0;i<N;i+=8){
            for(j=0;j<M;j+=8){
                for(n=i;n<i+4;n++){
                    for(m=0;m<8;m++){
                        tmp[m] = A[n][j+m];         
                    }
                    for(m=0;m<4;m++){
                        B[j+m][n] = tmp[m];
                        B[j+m][n+4] = tmp[m+4];  //upper right
                    }
                }
                for(m=j;m<j+4;m++){
                    for(n=4;n<8;n++){
                        tmp[n] = B[m][i+n];
                    }
                    for(n=4;n<8;n++){
                        B[m][i+n] = A[i+n][m];
                    }
                    for(n=0;n<4;n++){
                        B[m+4][i+n] = tmp[4+n]; //evictions here
                    }         
                }
                for(n=i+4;n<i+8;n++){  //lower right
                    for(m=j;m<j+4;m++){
                        B[m+4][n] = A[n][m+4];
                    }
                }
            }
        }
    }

    //61*67: similar to 32*32, set blocks to 16*8, see if n<67 in loops
    if(M==61&&N==67){ 
        for(i=0;i<N;i+=16){
            for(j=0;j<M;j+=8){
                for(n=i;n<i+16&&n<N;n++){
                    for(m=0;m<8;m++){
                        tmp[m] = A[n][j+m];
                    }
                    for(m=0;m<8;m++){
                        B[j+m][n] = tmp[m];
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




