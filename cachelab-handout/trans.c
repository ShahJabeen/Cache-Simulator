/*
  Programmer Name = Shah Jabeen Sajid
  File Name =  trans.c
  Date = 16- February 2018
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
   int size;
   int row,col, i, j;
   int a;

   // for 32 X 32 matrix
   if( M == 32 && N == 32)
   {
      size = 8; // block sizes  are better to use to reduce misses. 
      for(row = 0; row <N ; row += size)
      {
	 for (col = 0; col < M ; col += size)
	 {
	    for (i = row; i <row +size && i <N ; ++i)
	    {
	       for (j= col; j<col + size && j < M ; ++j)
	       {
		  if( i != j)
		  {
		     a = A[i][j];
		     B[j][i] = a;
		  }
	       }
	       if(row == col)
	       {
		  a = A[i][i];
		  B[i][i] = a;
	       }
	    }
	 }
      }
      
   }
   // for 64 X 64 matrix
   if(M == 64 && N == 64)
   {
      int i,j ,k,l,b;
      int C[7];
      
      size = 4;
      for(i = 0; i< N; i += size +size)
      {
	 for(j = 0; j < M ; j += size +size)
	 {
	    for (k = i ; k < i + 4; k++)
	    {
	       for(int a = 0; a < size + size ; a++)
	       {
		  C[a]= A[k][j + a];
	       }
	      
	       for(int a = 0; a <size ; a++)
	       {
		  B[j+ a][k] = C[a];
	       }
	       b= size;
	       for(int a = 0 ; a <size ; a++)
	       {
		  B[j+a][k + size] = C[b];
		  b++;
	       }
	
	    }
	    for(l= j+size; l< j+size+ size; l++)
	    {
	       for (int a =size; a<size +size ; a++)
	       {
		  C[a] = A[i +a][l - size];
	       }
	     
	       for(int a = 0; a < size ; a++)
	       {
		  C[a] = B[l - size][i + a + size];
	       }
	       
	       for(int a = size; a <size+size; a++)
	       {
		  B[ l - size][i + a]= C[a];
	       }
	      
	       for( int a = 0 ; a<size; a++)
	       {
		  B[l][i+a] = C[a];
	       }
	       
	       for(a = size; a < size + size ; a++)
	       {
		  B[l][i + a] = A[i + a][l];
	       }
	       
	    }
	 }
      }

   }
   // for 61 x 67 matrix
   if( M== 61 && N == 67)
   {
      size = 18;
      for(row = 0; row <N; row +=size)
      {
	 for(col=0; col<M; col+=size)
	 {
	    for(i= row; i<row +size && i <N ; ++i)
	    {
	       for(j=col; j<col +size && j <M ;++j)
	       {
		  a = A[i][j];
		  B[j][i] = a;
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

