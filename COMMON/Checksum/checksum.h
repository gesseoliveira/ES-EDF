/* *****************************************************************************
 FILE_NAME:     Checksum.h
 DESCRIPTION:   check routines
 DESIGNER:      Renato laureano
 CREATION_DATE: 07/aug/2015
 VERSION:       1.2
***************************************************************************** */

#ifndef CHECK_H
#define CHECK_H


/* *****************************************************************************
 *        INCLUDES
***************************************************************************** */
#include "Types.h"
#include "returncode.h"


/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define COMMON_CHECKSUM_VER_MAJOR   1
#define COMMON_CHECKSUM_VER_MINOR   2
#define COMMON_BRANCH_MASTER



/* *****************************************************************************
 *        DEFINES, ENUMS, STRUCT
***************************************************************************** */



/* *****************************************************************************
 *        VARIABLES
***************************************************************************** */



/* *****************************************************************************
 *        PROTOTYPES
***************************************************************************** */
uint8         CheckSum(uint8 *pData, uint16 Length);              /* calculates the  check sum complemented value of a memory data */
uint8         Checksum_8bits(uint8 *DataIn, uint8 SizeData);      /* similar to CheckSum, 8bit vars only */
uint16        CRC16(uint8 *pData, uint16 Length);                 /* calculates the CRC value of a memory data */
uint8         CRC8 (uint8 *pData, uint16 Length);                 /* calculates the CRC value of a memory data */
ReturnCode_t  CheckParity(uint8 *pData, uint16 Length);           /* check if every byte has the same parity */

#endif  /* CHECK_H */


















