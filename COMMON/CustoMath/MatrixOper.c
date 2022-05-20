/* *****************************************************************************
 FILE_NAME:     MatrixOper.c
 DESCRIPTION:   Software library with math functions used to perform matrix
                operations
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 20/feb/2017
 VERSION:       1.0
********************************************************************************
Version 1.0:    20/feb/2017 - Juliano Varasquim
                - First version of the source code
***************************************************************************** */




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "MatrixOper.h"







/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */





/* *****************************************************************************
 *
 *        GLOBAL VARIABLE AREAS
 *
***************************************************************************** */





/* *****************************************************************************
 *
 *        FUNCTIONS AREA
 *
***************************************************************************** */




/* -----------------------------------------------------------------------------
pivot()
        Consider the illustration below as a representation of square matrix
        called as A, and its square sub matrix called as S.

        A[0,0]------------------------------------------A[0,n]      B[0]
        |                                               |           | | 
        |                                               |           | | 
        |       S[l,l]------------------------S[l,m]    |           | | 
        |       |                             |         |           | | 
        |       |                             |         |           | | 
        |       |                             |         |           | | 
        |       |                             |         |           | | 
        |       S[m,l]------------------------S[m,m]    |           | | 
        |                                               |           | | 
        |                                               |           | | 
        A[n,0]------------------------------------------A[n,n]      B[n]

        The function checks all the values of the column 'l' into the sub matrix
        'S' and moves the row in which the higher value was found to the row 'l'.
        Also moves the values of the matrix B to keep the correlation between A
        and B.

--------------------------------------------------------------------------------
Input:
        l
          Index of initial row and initial column of the sub matrix 'S'.
        m
          Index of final row and final column of the sub matrix 'S'.
        n
          Row amount of the matrix 'A' which must be the same that 'B'.

In/Output:
        Ann
          Pointer to the first element of a two-dimensional matrix with
          'n' rows and 'n' columns.
        Bn1
          Pointer to the first element of a one-dimensional matrix with
          'n' lines and only one column.
Return:
        NA

WARNING:
        Notice that 'Ann' an 'Bn1' may be changed by the function, since the
        address of them is sent as parameter, which allows the function change
        the information directly in memory position of them.
----------------------------------------------------------------------------- */
void pivot( int32 l, int32 m, float *Ann, float *Bn1, int32 n)
  {
  int32 i, lMax;
  float AnnMax, tmp1, tmp2;

  AnnMax = AsMatrix(Ann,l,l,n);
  lMax = l;
  for( i = l; i < m; i++ )
    {
    if( fabsf( AnnMax ) < fabsf( AsMatrix(Ann,i,l,n) ) )
      {
      AnnMax = AsMatrix(Ann,i,l,n);
      lMax = i;
      }
    }

  if( l != lMax )
    {
    for( i = l; i < m; i++ )
      {
      tmp1 = AsMatrix(Ann,l,i,n);
      AsMatrix(Ann,l,i,n) = AsMatrix(Ann,lMax,i,n);
      AsMatrix(Ann,lMax,i,n) = tmp1;
      }
    tmp2 = Bn1[l];
    Bn1[l] = Bn1[lMax];
    Bn1[lMax] = tmp2;
    }

  return;
  }



