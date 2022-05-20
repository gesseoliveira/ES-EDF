/* ************************************************************************************************************
 FILE_NAME:     Endianness.c
 DESCRIPTION:   Support routines to perform the endianness change
 DESIGNER:      Denis Beraldo
 CREATION_DATE: 04/aug/2015
 VERSION:       1.0
**************************************************************************************************************
Version 1.1:  03/sep/2015 - Juliano Varasquim
              - Added the function that performs endianness change in 64 bits variables 
************************************************************************************************************ */
#include "Endianness.h"


inline __attribute__ ((always_inline)) uint16
Swap_uint16(uint16 datain)
{
  uint16 dataout;

  dataout = ( (((uint16)(datain) & (uint16)0x00FF) << 8)  |
              (((uint16)(datain) & (uint16)0xFF00) >> 8)  );

  return dataout;
}


inline __attribute__ ((always_inline)) uint32
Swap_uint32(uint32 datain)
{
  uint32 dataout;

  dataout = ( (((uint32)(datain) & (uint32)0x000000FFUL) << 24) |
              (((uint32)(datain) & (uint32)0x0000FF00UL) << 8)  |
              (((uint32)(datain) & (uint32)0x00FF0000UL) >> 8)  |
              (((uint32)(datain) & (uint32)0xFF000000UL) >> 24) );

  return dataout;
}


inline __attribute__ ((always_inline)) uint64
Swap_uint64(uint64 datain)
  {
  uint64 dataout;
  
  dataout = ( (((uint64)(datain) & (uint64)0x00000000000000FFUL) << 56) |
              (((uint64)(datain) & (uint64)0x000000000000FF00UL) << 40) |
              (((uint64)(datain) & (uint64)0x0000000000FF0000UL) << 24) |
              (((uint64)(datain) & (uint64)0x00000000FF000000UL) <<  8) |
              (((uint64)(datain) & (uint64)0x000000FF00000000UL) >>  8) |
              (((uint64)(datain) & (uint64)0x0000FF0000000000UL) >> 24) |
              (((uint64)(datain) & (uint64)0x00FF000000000000UL) >> 40) |
              (((uint64)(datain) & (uint64)0xFF00000000000000UL) >> 56) );

  return dataout;
  }
