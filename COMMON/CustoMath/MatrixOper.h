/* *****************************************************************************
 FILE_NAME:     MatrixOper.h
 DESCRIPTION:   Software library with math functions used to perform matrix
                operations
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 20/feb/2017
 VERSION:       1.0
***************************************************************************** */
#ifndef MATRIXOPER_H_INCLUDED
#define MATRIXOPER_H_INCLUDED




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "types.h"
#include "returncode.h"
#include "macros.h"
#include <math.h>



/* *****************************************************************************
 *
 *        FIRMWARE VERSION
 *
***************************************************************************** */
#define COMMON_MATRIXOPER_VER_MAJOR  1
#define COMMON_MATRIXOPER_VER_MINOR  0
#define COMMON_MATRIXOPER_BRANCH_MASTER




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */

/* -----------------------------------------------------------------------------
AsMatrix()
        The macro is used to access the data pointed from a pointer as if they
        had came from a two-dimensional matrix.
--------------------------------------------------------------------------------
Input:  Ptr
          Pointer to the first position of a float matrix
        Row
          Index of the line of the matrix which it is intended to access
        Col
          Index of the column of the matrix which it is intended to access
        ColMax
          Higher index of column of the matrix which was pointed by 'Ptr'.

 EXAMPLE:
        #define r ...                 // Row amount of the matrix definition
        #define c ...                 // Column amount of the matrix definition

        float M[r][c] = {...          // Matrix initialization
        float *pM;                    // Pointer to the matrix definition
        float Val;                    // Auxiliar variable definition

        pM = &M[0][0];                // Pointer to the matrix initialization

        Val = AsMatrix(pM, 0, 0, c);  // The same as: Val = M[0][0]
        Val = AsMatrix(pM, 0, 2, c);  // The same as: Val = M[0][2]
        Val = AsMatrix(pM, 1, 0, c);  // The same as: Val = M[1][0]
        Val = AsMatrix(pM, 2, 4, c);  // The same as: Val = M[2][4]

----------------------------------------------------------------------------- */
#define AsMatrix(Ptr, Row, Col, ColMax) (*(Ptr + (Row * ColMax + Col) ) )


/* *****************************************************************************
 *
 *        PROTOTYPES
 *
***************************************************************************** */
void pivot( int32 l, int32 m, float *Ann, float *Bn1, int32 n);




#endif /* MATRIXOPER_H_INCLUDED */
