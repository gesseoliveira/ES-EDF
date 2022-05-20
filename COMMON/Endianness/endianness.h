/* ************************************************************************************************************
 FILE_NAME:     Endianness.h
 DESCRIPTION:   Support routines to perform the endianness change
 DESIGNER:      Denis Beraldo
 CREATION_DATE: 04/aug/2015
 VERSION:       1.0
**************************************************************************************************************
Version 1.1:  03/sep/2015 - Juliano Varasquim
              - Added the function that performs endianness change in 64 bits variables
************************************************************************************************************ */

#ifndef SOURCES_COMMON_ENDIANNESS_H_
#define SOURCES_COMMON_ENDIANNESS_H_

#include "Types.h"

/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define COMMON_ENDIANNESS_VER_MAJOR		1
#define COMMON_ENDIANNESS_VER_MINOR		1
#define COMMON_BRANCH_MASTER

uint16 Swap_uint16(uint16 datain);
uint32 Swap_uint32(uint32 datain);
uint64 Swap_uint64(uint64 datain);


#endif /* SOURCES_COMMON_ENDIANNESS_H_ */
