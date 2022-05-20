/* ************************************************************************************************************
 FILE_NAME:     Crypt.h
 DESCRIPTION:   Software library with cryptographic cyphers
 DESIGNER:      Andre F. N. Dainese
 CREATION_DATE: 19/jul/2016
 VERSION:       1.0
************************************************************************************************************ */
#ifndef CRYPT_H_INCLUDED
#define CRYPT_H_INCLUDED




/* ************************************************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
************************************************************************************************************ */
#include "Types.h"
#include "ReturnCode.h"
#include "Macros.h"


/* *****************************************************************************
 *
 *        FIRMWARE VERSION
 *
***************************************************************************** */
#define COMMON_CRYPT_VER_MAJOR	1
#define COMMON_CRYPT_VER_MINOR	0
#define COMMON_BRANCH_MASTER



/* ************************************************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 ************************************************************************************************************ */


/* ************************************************************************************************************
 *
 *        PROTOTYPES
 *
************************************************************************************************************ */
ReturnCode_t XXTEA_Encrypt( uint32 * Data, uint32 Length, uint32 Key[4] );
ReturnCode_t XXTEA_Decrypt( uint32 * Data, uint32 Length, uint32 Key[4] );

#endif /* CRYPT_H_INCLUDED */