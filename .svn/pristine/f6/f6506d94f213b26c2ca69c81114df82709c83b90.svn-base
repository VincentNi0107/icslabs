//518021910834 ZHENYANG NI
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
    if(M==32&&N==32){
        int i,j,k,v0,v1,v2,v3,v4,v5,v6,v7;
        for(i=0;i<M;i+=8){
            for(j=0;j<N;j+=8){
                for(k=0;k<8;k++){
                    v0=A[i+k][j];
                    v1=A[i+k][j+1];
                    v2=A[i+k][j+2];
                    v3=A[i+k][j+3];
                    v4=A[i+k][j+4];
                    v5=A[i+k][j+5];
                    v6=A[i+k][j+6];
                    v7=A[i+k][j+7];
                    B[j+k][i]=v0;
                    B[j+k][i+1]=v1;
                    B[j+k][i+2]=v2;
                    B[j+k][i+3]=v3;
                    B[j+k][i+4]=v4;
                    B[j+k][i+5]=v5;
                    B[j+k][i+6]=v6;
                    B[j+k][i+7]=v7;
                }
                for(k=0;k<8;k++){
                    for(v0=k+1;v0<8;v0++){
                        v1=B[j+k][i+v0];
                        B[j+k][i+v0]=B[j+v0][i+k];
                        B[j+v0][i+k]=v1;
                    }
                }
            }
        }
        return;
    }
    else if(M==64&&N==64){
        int i,j,m,v0,v1,v2,v3,v4,v5,v6,v7;
        for(i=0;i<M;i+=8){
            for(j=0;j<N;j+=8){
                if(i==j&&i!=56){
                    for(m=0;m<4;m++){
                        v0=A[i+4+m][j];
                        v1=A[i+4+m][j+1];
                        v2=A[i+4+m][j+2];
                        v3=A[i+4+m][j+3];
                        v4=A[i+4+m][j+4];
                        v5=A[i+4+m][j+5];
                        v6=A[i+4+m][j+6];
                        v7=A[i+4+m][j+7];
                        B[j][i+m+8]=v0;
                        B[j+1][i+m+8]=v1;
                        B[j+2][i+m+8]=v2;
                        B[j+3][i+m+8]=v3;
                        B[j][i+m+12]=v4;
                        B[j+1][i+m+12]=v5;
                        B[j+2][i+m+12]=v6;
                        B[j+3][i+m+12]=v7;
                    }
                    for(m=0;m<4;m++){
                        v0=A[i+m][j];
                        v1=A[i+m][j+1];
                        v2=A[i+m][j+2];
                        v3=A[i+m][j+3];
                        v4=A[i+m][j+4];
                        v5=A[i+m][j+5];
                        v6=A[i+m][j+6];
                        v7=A[i+m][j+7];
                        B[j][i+m]=v0;
                        B[j+1][i+m]=v1;
                        B[j+2][i+m]=v2;
                        B[j+3][i+m]=v3;
                        B[j][i+m+4]=v4;
                        B[j+1][i+m+4]=v5;
                        B[j+2][i+m+4]=v6;
                        B[j+3][i+m+4]=v7;
                    }
                    for(m=0;m<4;m++){
                        v0=B[j+m][i+4];
                        v1=B[j+m][i+5];
                        v2=B[j+m][i+6];
                        v3=B[j+m][i+7];
                        B[j+m][i+4]=B[j+m][j+8];
                        B[j+m][i+5]=B[j+m][j+9];
                        B[j+m][i+6]=B[j+m][j+10];
                        B[j+m][i+7]=B[j+m][j+11];
                        B[j+4+m][i]=v0;
                        B[j+4+m][i+1]=v1;
                        B[j+4+m][i+2]=v2;
                        B[j+4+m][i+3]=v3;
                    }
                    for(m=0;m<4;m++){
                        v0=B[j+m][i+12];
                        v1=B[j+m][i+13];
                        v2=B[j+m][i+14];
                        v3=B[j+m][i+15];
                        B[j+4+m][i+4]=v0;
                        B[j+4+m][i+5]=v1;
                        B[j+4+m][i+6]=v2;
                        B[j+4+m][i+7]=v3;
                    }
                    continue;
                }
                for(m=0;m<4;m++){
                    v0=A[i+m][j];
                    v1=A[i+m][j+1];
                    v2=A[i+m][j+2];
                    v3=A[i+m][j+3];
                    v4=A[i+m][j+4];
                    v5=A[i+m][j+5];
                    v6=A[i+m][j+6];
                    v7=A[i+m][j+7];
                    B[j][i+m]=v0;
                    B[j+1][i+m]=v1;
                    B[j+2][i+m]=v2;
                    B[j+3][i+m]=v3;
                    B[j][i+m+4]=v4;
                    B[j+1][i+m+4]=v5;
                    B[j+2][i+m+4]=v6;
                    B[j+3][i+m+4]=v7;
                }
                for(m=0;m<4;m++){
                    v0=B[j+m][i+4];
                    v1=B[j+m][i+5];
                    v2=B[j+m][i+6];
                    v3=B[j+m][i+7];
                    v4=A[i+4][j+m];
                    v5=A[i+5][j+m];
                    v6=A[i+6][j+m];
                    v7=A[i+7][j+m];

                    B[j+m][i+4]=v4;
                    B[j+m][i+5]=v5;
                    B[j+m][i+6]=v6;
                    B[j+m][i+7]=v7;
                    B[j+4+m][i]=v0;
                    B[j+4+m][i+1]=v1;
                    B[j+4+m][i+2]=v2;
                    B[j+4+m][i+3]=v3;
                }
                for(m=0;m<4;m++){
                    v0=A[i+4+m][j+4];
                    v1=A[i+4+m][j+5];
                    v2=A[i+4+m][j+6];
                    v3=A[i+4+m][j+7];
                    B[j+4][i+4+m]=v0;
                    B[j+5][i+4+m]=v1;
                    B[j+6][i+4+m]=v2;
                    B[j+7][i+4+m]=v3;
                }
            }
        }
        return;
    }
    else{
        int i,j,m,n;
        for(i=0;i<N;i+=16){
            for(j=0;j<M;j+=16){
                for(n=i;n<i+16&&n<N;n++){
                    for(m=j;m<j+16&&m<M;m++){
                        B[m][n]=A[n][m];
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

