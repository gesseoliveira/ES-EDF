/* *****************************************************************************
 FILE_NAME:     WaveFit.c
 DESCRIPTION:   Software library with math functions used to perform wave 
                fitting
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
#include "WaveFit.h"
#include "MatrixOper.h"

/* *****************************************************************************
 *
 *        LOCAL PROTOTYPE AREA
 *
 ***************************************************************************** */
static inline float fx ( float x , float *a, int32 Ord );


/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */
#define p         6
#define n         2

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
PolyFit()
        Returns a ...
--------------------------------------------------------------------------------
Input:  xDat
          Pointer to a float point data array used as the set of independent
          variables
        yDat
          Pointer to a float point data array used as the set of dependent
          variables
        DatLen
          Length of the float point data vectors xDat and yDat
        PolOrd
          Integer value which indicates the maximum order of the polynomial
          function. Some examples are left below:
            1 =>      y = a.x + b                     (1st order polynomial)
            2 =>      y = a.x² + b.x + c              (2nd order polynomial)
            3 =>      y = a.x³ + b.x² + c.x + d       (3rd order polynomial)
Output: fDat
          Pointer to a float point data array used as fitted data output. Note,
          if a NULL pointer is passed as parameter, this functionality will be
          not processed.
        pR2
          Pointer to the float variable which will receive the fitting
          coefficient R²
        pCoef
          Pointer to a float point data array which will receive the polynomial
          coefficients
Return: ERR_PARAM_ADDRESS
          Some of the required pointers to the data array was declared as NULL
        ERR_PARAM_BUFFER_COUNT
          The data array length is bigger than the function is able to handle
        ERR_PARAM_SIZE
          The order of the polynomial passed as parameter is higher than the
          function is able to handle
        ANSWERED_REQUEST
          The operation has been successfully performed
----------------------------------------------------------------------------- */
ReturnCode_t PolyFit(float xDat[], float yDat[], int32 DatLen, int32 PolOrd, float fDat[], float *pR2, float pCoef[])
{
  ReturnCode_t  ReturnValue;
  float A[MAX_POLYFIT_ORD+1][MAX_POLYFIT_ORD+1];  // Coefficient Matrix
  float B[MAX_POLYFIT_ORD+1];                     // Vector of Independent Terms
  float a[MAX_POLYFIT_ORD+1];                     // Regression Coefficients
  float aux;                                      // Auxiliary float variable
  float Sy = 0.0F, Se = 0.0F, Sye = 0.0F;
  float e;
  float SQReg = 0.0F, SQRes = 0.0F, SQT;
  int32 c, i, j, k, l;
  int32 PolOrdLen;
  float ym;

  /* Sanity checks                                                            */
  if( (xDat == NULL)
      ||(yDat == NULL) )
  {
    /* If some of the pointers was not initialized                            */
    ReturnValue = ERR_PARAM_ADDRESS;
  }
  else if(DatLen > MAX_POLYFIT_LEN)
  {
    /* If the length of the buffer is too big                                 */
    ReturnValue = ERR_PARAM_BUFFER_COUNT;
  }
  else if(PolOrd > MAX_POLYFIT_ORD)
  {
    /* If the polynomial order is too high                                    */
    ReturnValue = ERR_PARAM_SIZE;
  }
  else
  {
    /* If all the parameters are OK carry on with the processing              */
    PolOrdLen = PolOrd + 1;

    /* Calculates the 'Coefficient Matrix' called as 'A'.                     */
    for( j = 0; j < PolOrdLen; j++ )
    {
      for( k = 0; k < PolOrdLen; k++ )
      {
        A[j][k] = 0.0;
        for( i = 0; i < DatLen; i++ )
        {
          A[j][k] += powf( xDat[i], (float)(j+k) );
        }
      }
    }

    /* Calculates the 'Vector of Independent Terms' called as 'B'.            */
    for( j = 0; j < PolOrdLen; j++ )
    {
      B[j] = 0.0;
      for( i = 0; i < DatLen; i++ )
      {
        B[j] += powf( xDat[i], (float)j ) * yDat[i];
      }
    }

    /* Performs the pivoting of the coefficient matrix                        */
    for( i= 0; i < PolOrdLen; i++ )
    {
      pivot( i, PolOrdLen, &A[0][0], &B[0], MAX_POLYFIT_ORD+1);  //////////////////////////////////////////////
      aux = A[i][i];
      for( j = 0; j < PolOrdLen; j++ )
      {
        if( aux != 0.0 ) A[i][j] /= aux;
      }

      if( aux != 0.0 ) B[i] /= aux;

      for( l = i+1; l < PolOrdLen; l++ )
      {
        aux = A[l][i];
        for( c = i; c < PolOrdLen; c++ )
        {
          A[l][c] -= aux * A[i][c];
        }
        B[l] -= aux * B[i];
      }
    }

    a[PolOrd] = B[PolOrd];

    for( l = (PolOrd - 1); l >= 0; l-- )
    {
      a[l] = B[l];
      for( c = l+1; c < PolOrdLen; c++ )
      {
        a[l] -= A[l][c] * a[c];
      }
    }

    /* Calculates the fitted function output, but only if an output vector    */
    /* was passed as parameter                                                */
    if(fDat != NULL)
    {
      for( i = 0; i < DatLen; i++ )
      {
        fDat[i] = fx( xDat[i], a, PolOrdLen);
      }
    }

    /* Copies the polynomial coefficients, but only if a valid pointer was    */
    /* passed as parameter                                                    */
    if(pCoef != NULL)
    {
      for( i = 0; i < PolOrdLen; i++ )
      {
        pCoef[i] = a[i];
      }
    }

    /* Calculates the R² of the function, but only if some valid variable was */
    /* pointed by pR2                                                         */
    if(pR2 != NULL)
    {
      for( i = 0; i < DatLen; i++ )
      {
        Sy += yDat[i];
        e = (yDat[i] - fDat[i]);
        Se += e;
        SQRes += (e * e);
        Sye += fDat[i];
      }

      ym = Sy / DatLen;

      for( i = 0; i < n; i++ )
      {
        e = fDat[i] - ym;
        SQReg += (e * e);
      }

      SQT = SQReg + SQRes;
      *pR2 = SQReg / SQT;
    }


    /* Function return                                                        */
    ReturnValue = ANSWERED_REQUEST;
  }


  /* Job done. */
  return ReturnValue;
}


//----------------------------------------------------------
static inline float fx ( float x , float *a, int32 Ord )
{
  int32 k;
  float y = 0.0F;

  for( k = 0; k < Ord; k++ )
  {
    y += a[k] * pow( x, (float) k );
  }

  return y;
}


//----------------------------------------------------------





