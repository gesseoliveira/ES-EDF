/*------------------------------------------------------------------------------
FILE_NAME:     VUR.h
DESCRIPTION:   Variable Update Register Driver
DESIGNER:      Andre F. N. Dainese
CREATION_DATE: 07/2015
VERSION:       2.3
------------------------------------------------------------------------------*/
#ifndef VUR_H
#define VUR_H

/* *****************************************************************************
 *        INCLUDES (and DEFINES for INCLUDES)
***************************************************************************** */
#include "../StdHeaders/returncode.h"
#include "../StdHeaders/types.h"

/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define COMMON_VUR_VER_MAJOR    2
#define COMMON_VUR_VER_MINOR    3
#define COMMON_BRANCH_MASTER

/* *****************************************************************************
 *        TYPE DEFINITION
***************************************************************************** */
typedef enum VarmapKind_e
  {
  MapUndef,
  MapConfig,
  MapOutput,
  MapLen      = 0xFFFF
  }VarmapKind_t;

typedef struct VarChange_s
  {
  uint16 VmpIdx;
  void (*Callback)(uint32, void *);
  void * Parameter;
  }VarChange_t;

/* Return Values                                               Error? |  Code */
#define VUR_REQUEST_ANSWERED                          ( (uint8)(0x00) | (0x00) )
#define VUR_TABLE_EMPTY                               ( (uint8)(0x01) | (0x01) )
#define VUR_TABLE_OVERFLOW                            ( (uint8)(0x01) | (0x01) )

/* *****************************************************************************
 *        FUNCTIONS / ROUTINES PROTOTYPES
***************************************************************************** */
uint8 VUR_AddItem(uint8 ID, uint16 VarMapIndex, uint16 TableAddress, void * Data_New, void * Data_Old, uint8 DataSize);
uint8 VUR_GetNextLogItem(uint8 * ID, uint16 * VarMapIndex, uint16 * TableAddress, uint32 * Data_New, uint32 * Data_Old);
uint8 VUR_GetNextVolatileItem(uint8 * ID, uint16 * VarMapIndex);
uint8 VUR_GetNextExternalItem(uint8 * ID, uint16 * VarMapIndex, uint32 * Data_New);
uint8 VUR_GetLogCount(uint16 * NumberOfItems);
uint8 VUR_GetVolatileCount(uint16 * NumberOfItems);
uint8 VUR_GetExternalCount(uint16 * NumberOfItems);

ReturnCode_t VUR_Subscribe( void *VarAddress, void (*Callback)(uint32, void *), void * Parameter, uint32 *Ticket );
ReturnCode_t VUR_Unsubscribe( uint32 Ticket );
ReturnCode_t VUR_FlagVariableWrite( void *VarAddress );

ReturnCode_t VUR_GetVarmapIndex(void *VarAddress, VarmapKind_t *MapKind, uint16 *Index);

#endif
