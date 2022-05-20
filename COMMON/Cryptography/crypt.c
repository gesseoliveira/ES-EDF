/* *****************************************************************************
 FILE_NAME:     Crypt.c
 DESCRIPTION:   Software library with cryptographic cyphers
 DESIGNER:      Andre F. N. Dainese
 CREATION_DATE: 19/jul/2016
 VERSION:       1.0
********************************************************************************
Version 1.0:    19/jul/2016 - Andre F. N. Dainese
                - First release
***************************************************************************** */




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "Crypt.h"

/* *****************************************************************************
 *
 *        PRIVATE_DEFINITIONS
 *
***************************************************************************** */
#define XXTEA_MX      (((z >> 5 ^ y << 2) + ( y >> 3 ^ z << 4 )) ^ (( sum ^ y ) + ( Key[ ( p & 3 ) ^ e ] ^ z)))
#define XXTEA_DELTA   0x9e3779b9


/* *****************************************************************************
 *
 *        FUNCTIONS AREA
 *
***************************************************************************** */
/* -----------------------------------------------------------------------------
XXTEA_Encrypt() - XXTEA, Encryption Algorithm
        This routine encrypts a buffer of data using XXTEA.
--------------------------------------------------------------------------------
Input / Output: Data - Buffer data contains the data to be encrypted. It'll be
                        overwritten with the encrypted result.
Input:          Length - Data Array Length
                Key    - Array containing the 128bit Key

Return: Result         - ANSWERED_REQUEST : Operation Successful
                         Else             : Error
--------------------------------------------------------------------------------
Note: Further details can be found on: https://en.wikipedia.org/wiki/XXTEA
----------------------------------------------------------------------------- */
ReturnCode_t XXTEA_Encrypt( uint32 * Data, uint32 Length, uint32 Key[4] )
{
  ReturnCode_t ReturnValue;
  uint32       y, z, sum;
  uint32       p, rounds, e;

  /* First, check if length is valid (not zero)                               */
  if( Length == 0 )
  {
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  {
    rounds = 6 + 52/Length;
    sum = 0;
    z = Data[Length-1];
    do 
    {
      sum += XXTEA_DELTA;
      e = (sum >> 2) & 3;
      for (p = 0; p < (Length - 1); p++)
      {
        y = Data[p + 1]; 
        z = Data[p] += XXTEA_MX;
      }
      y = Data[0];
      z = Data[Length - 1] += XXTEA_MX;
    } while (--rounds);

    /* Job done.                                                              */
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/* -----------------------------------------------------------------------------
XXTEA_Encrypt() - XXTEA, Decryption Algorithm
        This routine decrypts a buffer of data using XXTEA.
--------------------------------------------------------------------------------
Input / Output: Data - Buffer data contains the data to be decrypted. It'll be
                        overwritten with the decrypted result.
Input:          Length - Data Array Length
                Key    - Array containing the 128bit Key

Return: Result         - ANSWERED_REQUEST : Operation Successful
                         Else             : Error
--------------------------------------------------------------------------------
Note: Further details can be found on: https://en.wikipedia.org/wiki/XXTEA
----------------------------------------------------------------------------- */
ReturnCode_t XXTEA_Decrypt( uint32 * Data, uint32 Length, uint32 Key[4] )
{
  ReturnCode_t ReturnValue;
  uint32       y, z, sum;
  uint32       p, rounds, e;

  /* First, check if length is valid (not zero)                               */
  if( Length == 0 )
  {
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  {
    rounds = 6 + 52 / Length;
    sum = rounds * XXTEA_DELTA;
    y = Data[0];
    do 
    {
      e = (sum >> 2) & 3;
      for (p = Length - 1; p > 0; p--) 
      {
        z = Data[p - 1];
        y = Data[p] -= XXTEA_MX;
      }
      z = Data[Length - 1];
      y = Data[0] -= XXTEA_MX;
      sum -= XXTEA_DELTA;
    } while (--rounds);

    /* Job done.                                                              */
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

