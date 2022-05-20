/*------------------------------------------------------------------------------
FILE_NAME:      VUR.c
DESCRIPTION:    This driver is responsible for managing the Variable Update
                Register. It consists of a circular buffer array where each
                position shall indicate a variable update, informing which is
                the Register Number, what is the new value and who has done the
                change (Modbus? Dust Mote? Others?).
DESIGNER:       Andre F. N. Dainese
VERSION:        2.3
------------------------------------------------------------------------------*/

/* *****************************************************************************
 *        INCLUDES (and DEFINES for INCLUDES)
 **************************************************************************** */
#define ENABLE_VUR_FUNCTIONS

#include "Types.h"
#include <string.h>
#include "VUR.h"
#include "VARMAP.h"
#include "SETUP.h"
#include "ReturnCode.h"
#include "macros.h"

/* *****************************************************************************
 *        DEFINES, ENUMS, STRUCT
***************************************************************************** */
#ifndef VUR_SIZE
  /* Set in the definition below the size (number of items) that will be able */
  /*  to be stored in the VUR Table.                                          */
  #define VUR_SIZE   ((uint16) 250)
#endif

#ifndef SUBSCR_LIST_SIZE
  /* Set in the definition below the size (number of items) that will be able */
  /*  to be stored in the VUR Table.                                          */
  #define SUBSCR_LIST_SIZE   ((uint16) 50)
#endif

typedef struct
{
  uint32      NewRegisterValue;
  uint32      OldRegisterValue;
  uint16      VarMapRegisterIndex;
  uint16      TableAddressValue;
  uint8       Source_ID;
} VUR_Log_t;

typedef struct
{
  uint16      VarMapRegisterIndex;
  uint8       Source_ID;
} VUR_Volatile_t;

typedef struct
{
  uint32      NewRegisterValue;
  uint16      VarMapRegisterIndex;
  uint8       Source_ID;
} VUR_External_t;


/* *****************************************************************************
 *        LOCAL PROTOTYPES
 **************************************************************************** */
static uint32       VUR_PrepareData(void * Data, uint8 DataSize);
static ReturnCode_t VUR_TriggerSubscriptionsFor( uint16 VarMapIndex, uint8 ID );

/* *****************************************************************************
 *        LOCAL VARIABLES AND CONSTANTS
 **************************************************************************** */
/* Below are declared the table itself, a constant representing its size and  */
/*  a variable indicating the next table position that is free to write.      */
VUR_Log_t           VUR_LogTable[VUR_SIZE];
uint16              VUR_LogLastPosPtr = 0;
uint16              VUR_LogCurrPosPtr = 0;

/* Store volatile variables. */
VUR_Volatile_t      VUR_VolTable[VUR_SIZE];
uint16              VUR_VolLastPosPtr = 0;
uint16              VUR_VolCurrPosPtr = 0;

/* Store external variables. */
VUR_External_t      VUR_ExtTable[VUR_SIZE];
uint16              VUR_ExtLastPosPtr = 0;
uint16              VUR_ExtCurrPosPtr = 0;

#define SUBSCR_INI  {0,NULL,NULL}
static VarChange_t  SubscrList[SUBSCR_LIST_SIZE] =
    INITIALIZE_VARIABLE(SUBSCR_LIST_SIZE,SUBSCR_INI);

/* *****************************************************************************
 *        PUBLIC FUNCTIONS AREA
 **************************************************************************** */
uint8 VUR_AddItem(uint8 ID, uint16 VarMapIndex, uint16 TableAddress, void * Data_New, void * Data_Old, uint8 DataSize)
{
  uint8 ReturnValue;

  /* First, check if given index is inside the config table range.            */
  if( VarMapIndex >= VarMapConfigLen )
  { /* Return an error value.                                                 */
    ReturnValue = VUR_TABLE_OVERFLOW;
  }
  else
  {
    bool VolatilePosHasChanged = FALSE;
    bool ExternalPosHasChanged = FALSE;
    bool LoggingPosHasChanged  = FALSE;

    /* If variable changed need to be stored.                                 */
    if (VarMapConfigTable[VarMapIndex].Volatility == NON_VOLATILE)
    {
      /* Add the given information to the table, in the line being pointed.   */
      VUR_VolTable[VUR_VolCurrPosPtr].VarMapRegisterIndex = VarMapIndex;
      VUR_VolTable[VUR_VolCurrPosPtr].Source_ID           = ID;

      /* Increment the pointer to the next position                           */
      if( VUR_VolCurrPosPtr >= ( VUR_SIZE - 1 ) )   { VUR_VolCurrPosPtr = 0; }
      else                                          { VUR_VolCurrPosPtr++;   }

      VolatilePosHasChanged = TRUE;
    }
    /* If variable changed is external                                        */
    else if (VarMapConfigTable[VarMapIndex].Volatility == EXTERNAL_VAR)
    {
      /* Add the given information to the table, in the line being pointed.   */
      VUR_ExtTable[VUR_ExtCurrPosPtr].NewRegisterValue    = VUR_PrepareData(Data_New, DataSize);
      VUR_ExtTable[VUR_ExtCurrPosPtr].VarMapRegisterIndex = VarMapIndex;
      VUR_ExtTable[VUR_ExtCurrPosPtr].Source_ID           = ID;

      /* Increment the pointer to the next position                           */
      if( VUR_ExtCurrPosPtr >= ( VUR_SIZE - 1 ) )   { VUR_ExtCurrPosPtr = 0; }
      else                                          { VUR_ExtCurrPosPtr++;   }

      ExternalPosHasChanged = TRUE;
    }

    /* If need to create a log for this variable.                             */
    if (VarMapConfigTable[VarMapIndex].LogChanges == LOG_CHANGES)
    {
      /* Add the given information to the table, in the line being pointed.   */
      VUR_LogTable[VUR_LogCurrPosPtr].NewRegisterValue    = VUR_PrepareData(Data_New, DataSize);
      VUR_LogTable[VUR_LogCurrPosPtr].OldRegisterValue    = VUR_PrepareData(Data_Old, DataSize);
      VUR_LogTable[VUR_LogCurrPosPtr].VarMapRegisterIndex = VarMapIndex;
      VUR_LogTable[VUR_LogCurrPosPtr].TableAddressValue   = TableAddress;
      VUR_LogTable[VUR_LogCurrPosPtr].Source_ID           = ID;

      /* Increment the pointer to the next position                           */
      if( VUR_LogCurrPosPtr >= ( VUR_SIZE - 1 ) )   { VUR_LogCurrPosPtr = 0; }
      else                                          { VUR_LogCurrPosPtr++;   }

      LoggingPosHasChanged = TRUE;
    }

    /* Check if there is some subscription related to the current map index   */
    {
      VUR_TriggerSubscriptionsFor( VarMapIndex, ID );
    }

    /* If the current pointer reaches the old one, we have a overflow event.  */
    if( ( ( VolatilePosHasChanged != FALSE ) && ( VUR_VolCurrPosPtr == VUR_VolLastPosPtr ) ) ||
        ( ( ExternalPosHasChanged != FALSE ) && ( VUR_ExtCurrPosPtr == VUR_ExtLastPosPtr ) ) ||
        ( ( LoggingPosHasChanged  != FALSE ) && ( VUR_LogCurrPosPtr == VUR_LogLastPosPtr ) ) )
    {
      DO_ONLY_ONCE( __DEBUGHALT(); );

      ReturnValue = VUR_TABLE_OVERFLOW;
    }
    else
    {
      ReturnValue = VUR_REQUEST_ANSWERED;
    }
  }

  /* Job done */
  return ReturnValue;
}




/*
 * Request items from log table.
 */
uint8 VUR_GetNextLogItem(uint8 * ID, uint16 * VarMapIndex, uint16 * TableAddress, uint32 * Data_New, uint32 * Data_Old)
{
  uint8 ReturnValue;
  /* This routine increments the OldPos pointer and returns the value         */
  /*  contained in its position.                                              */

  /* First, check if there is data available in the VUR */
  if( VUR_LogCurrPosPtr != VUR_LogLastPosPtr )
  { /* There is data to deliver */
    /* Get the value currently being pointed to */
    *ID           = VUR_LogTable[VUR_LogLastPosPtr].Source_ID;
    *VarMapIndex  = VUR_LogTable[VUR_LogLastPosPtr].VarMapRegisterIndex;
    *TableAddress = VUR_LogTable[VUR_LogLastPosPtr].TableAddressValue;
    *Data_New     = VUR_LogTable[VUR_LogLastPosPtr].NewRegisterValue;
    *Data_Old     = VUR_LogTable[VUR_LogLastPosPtr].OldRegisterValue;

    /* Increment the pointer for next value */
    if( VUR_LogLastPosPtr >= ( VUR_SIZE - 1 ) ) { VUR_LogLastPosPtr = 0; }
    else                                        { VUR_LogLastPosPtr++;   }

    ReturnValue = VUR_REQUEST_ANSWERED;
  }
  else
  { /* VUR is empty */
    ReturnValue = VUR_TABLE_EMPTY;
  }

  return ReturnValue;
}




/*
 * Request items from volatile table.
 */
uint8 VUR_GetNextVolatileItem(uint8 *ID, uint16 *VarMapIndex)
{
  uint8 ReturnValue;
  /* This routine increments the OldPos pointer and returns the value         */
  /*  contained in its position.                                              */

  /* First, check if there is data available in the VUR */
  if( VUR_VolCurrPosPtr != VUR_VolLastPosPtr )
  { /* There is data to deliver */
    /* Get the value currently being pointed to */
    *ID           = VUR_VolTable[VUR_VolLastPosPtr].Source_ID;
    *VarMapIndex  = VUR_VolTable[VUR_VolLastPosPtr].VarMapRegisterIndex;

    /* Increment the pointer for next value */
    if( VUR_VolLastPosPtr >= ( VUR_SIZE - 1 ) ) { VUR_VolLastPosPtr = 0; }
    else                                        { VUR_VolLastPosPtr++;   }

    ReturnValue = VUR_REQUEST_ANSWERED;
  }
  else
  { /* VUR is empty */
    ReturnValue = VUR_TABLE_EMPTY;
  }

  return ReturnValue;
}




/*
 * Request items from external table.
 */
uint8 VUR_GetNextExternalItem(uint8 * ID, uint16 * VarMapIndex, uint32 * Data_New)
{
  uint8 ReturnValue;
  /* This routine increments the OldPos pointer and returns the value         */
  /*  contained in its position.                                              */

  /* First, check if there is data available in the VUR */
  if( VUR_ExtCurrPosPtr != VUR_ExtLastPosPtr )
  { /* There is data to deliver */
    /* Get the value currently being pointed to */
    *Data_New     = VUR_ExtTable[VUR_ExtLastPosPtr].NewRegisterValue;
    *ID           = VUR_ExtTable[VUR_ExtLastPosPtr].Source_ID;
    *VarMapIndex  = VUR_ExtTable[VUR_ExtLastPosPtr].VarMapRegisterIndex;

    /* Increment the pointer for next value */
    if( VUR_ExtLastPosPtr >= ( VUR_SIZE - 1 ) ) { VUR_ExtLastPosPtr = 0; }
    else                                        { VUR_ExtLastPosPtr++;   }

    ReturnValue = VUR_REQUEST_ANSWERED;
  }
  else
  { /* VUR is empty */
    ReturnValue = VUR_TABLE_EMPTY;
  }

  return ReturnValue;
}




/*
 * Return the number of items
 *  stored at log table.
 */
uint8 VUR_GetLogCount(uint16 * NumberOfItems)
{
  uint8 ReturnValue;
  uint16 NumberCount;

  /* Return how many items are available for reading in the table. Remembering*/
  /*  that this is a circular buffer, so the item count will take in          */
  /*  consideration both pointers.                                            */
  /* If the table has items, return regular answer. If table is empty, though,*/
  /*  then return this information */
  if( VUR_LogCurrPosPtr >= VUR_LogLastPosPtr )  { NumberCount = VUR_LogCurrPosPtr - VUR_LogLastPosPtr;            }
  else                                          { NumberCount = VUR_SIZE + VUR_LogCurrPosPtr - VUR_LogLastPosPtr; }

  if( NumberCount != 0 )  { ReturnValue = VUR_REQUEST_ANSWERED; }
  else                    { ReturnValue = VUR_TABLE_EMPTY;      }

  *NumberOfItems = NumberCount;

  return ReturnValue;
}




/*
 * Return the number of items
 *  stored at volatile table.
 */
uint8 VUR_GetVolatileCount(uint16 * NumberOfItems)
{
  uint8 ReturnValue;
  uint16 NumberCount;

  /* Return how many items are available for reading in the table. Remembering*/
  /*  that this is a circular buffer, so the item count will take in          */
  /*  consideration both pointers.                                            */
  /* If the table has items, return regular answer. If table is empty, though,*/
  /*  then return this information */
  if( VUR_VolCurrPosPtr >= VUR_VolLastPosPtr )  { NumberCount = VUR_VolCurrPosPtr - VUR_VolLastPosPtr;            }
  else                                          { NumberCount = VUR_SIZE + VUR_VolCurrPosPtr - VUR_VolLastPosPtr; }

  if( NumberCount != 0 )  { ReturnValue = VUR_REQUEST_ANSWERED; }
  else                    { ReturnValue = VUR_TABLE_EMPTY;      }

  *NumberOfItems = NumberCount;

  return ReturnValue;
}

/*
 * Return the number of items
 *  stored at external table.
 */
uint8 VUR_GetExternalCount(uint16 * NumberOfItems)
{
  uint8 ReturnValue;
  uint16 NumberCount;

  /* Return how many items are available for reading in the table. Remembering*/
  /*  that this is a circular buffer, so the item count will take in          */
  /*  consideration both pointers.                                            */
  /* If the table has items, return regular answer. If table is empty, though,*/
  /*  then return this information */
  if( VUR_ExtCurrPosPtr >= VUR_ExtLastPosPtr )  { NumberCount = VUR_ExtCurrPosPtr - VUR_ExtLastPosPtr;            }
  else                                          { NumberCount = VUR_SIZE + VUR_ExtCurrPosPtr - VUR_ExtLastPosPtr; }

  if( NumberCount != 0 )  { ReturnValue = VUR_REQUEST_ANSWERED; }
  else                    { ReturnValue = VUR_TABLE_EMPTY;      }

  *NumberOfItems = NumberCount;

  return ReturnValue;
}

/* -----------------------------------------------------------------------------
VUR_GetVarmapIndex()
        Looks for a memory position into the VARMAP and return its index at the
        map when it is found.
--------------------------------------------------------------------------------
Input:
        VarAddress
          Pointer to a memory position that must belongs to the VARMAP
In/Output:
        MapKind
          Pointer to a variable which indicates which of VARMAP will be used at
          the lookup process. When the special value 'MapUndef' is the value of
          the pointed variable the lookup process is performed at both maps
Output: Index
          Pointer to the variable which will receive the index from the map
          which has the memory address pointed by the 'VarAddress' variable

Return: ANSWERED_REQUEST
          Process complete
        ERR_PARAM_DATA
          Operation has been failed, the memory position was not found into the
          VARMAP
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
ReturnCode_t VUR_GetVarmapIndex(void *VarAddress,VarmapKind_t *MapKind ,uint16 *Index)
{
  #define NO_ONE_MAP                                                        0x00
  #define CONFIG_MAP                                                        0x01
  #define OUTPUT_MAP                                                        0x02

  uint8 LookVarAt;
  uint16 Idx;
  ReturnCode_t FuncRet = ERR_PARAM_DATA;


  /* Defines the maps which will be used to the lookup process of address of  */
  /* variable                                                                 */
  switch(*MapKind)
    {
    case MapUndef:
      {
      LookVarAt = CONFIG_MAP | OUTPUT_MAP;
      }
      break;

    case MapConfig:
      {
      LookVarAt = CONFIG_MAP;
      }
      break;

    case MapOutput:
      {
      LookVarAt = OUTPUT_MAP;
      }
      break;

    default:
      {
      LookVarAt = NO_ONE_MAP;
      }
      break;
    }


  /* Performs the lookup at the config map only if this process is enabled    */
  if(LookVarAt & CONFIG_MAP)
    {
    for(Idx = 0; Idx < VarMapConfigLen; Idx++) {
      if(VarMapConfigTable[Idx].VarAddress == VarAddress)
        {
        /* The address of the variable was found, so performs the sequence    */
        /* required to leave the loop with safety                             */
        LookVarAt = NO_ONE_MAP;
        *MapKind = MapConfig;
        *Index = Idx;
        FuncRet = ANSWERED_REQUEST;
        break;
        }
      }
    }


  /* Performs the lookup at the output map only if this process is enabled    */
  if(LookVarAt & OUTPUT_MAP)
    {
    for(Idx = 0; Idx < VarMapOutputLen; Idx++){
      if(VarMapOutputTable[Idx].VarAddress == VarAddress)
        {
        /* The address of the variable was found, so performs the sequence    */
        /* required to leave the loop with safety                             */
        LookVarAt = NO_ONE_MAP;
        *MapKind = MapOutput;
        *Index = Idx;
        FuncRet = ANSWERED_REQUEST;
        break;
        }
      }
    }


  /* Return the function status result                                        */
  return FuncRet;
}




/* -----------------------------------------------------------------------------
VUR_Subscribe()
        Receives a pointer to a function and a pointer to a variable as
        parameters and build a mechanism which execute the pointed function
        every time the pointed variable is changed
--------------------------------------------------------------------------------
Input:  VarAddress
          Pointer to a memory position which will be used as trigger to a
          function execution when the variable placed at that position be
          changed. This variable must belongs to the VARMAP.
        Callback
          Pointer to a function which will be automatically executed when the
          variable at the memory position pointed by the pointer above is
          changed
        Parameter
          Some data to be given to the callback routine when it is called due
          to this subscription. It can be a pointer to something, a value
          (properly casted) or NULLL.

Output: Ticket
          Index gotten by the subscription at the subscription list. If no
          variable is given to receive it (NULL), logic understands and does't
          return anything here.

Return: ANSWERED_REQUEST
          Process completed successfully
        ERR_PARAM_DATA
          The address of the variable used as trigger to the function is invalid
        ERR_PARAM_COMMAND
          The address of the function executed when the variable changes is
          invalid
        ERR_QFULL
          The subscription list already is fully filled
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
ReturnCode_t VUR_Subscribe( void *VarAddress, void (*Callback)(uint32,void *), void * Parameter, uint32 *Ticket )
  {
  ReturnCode_t InternRet;
  ReturnCode_t SubscProcess;
  VarmapKind_t MapKind = MapConfig;
  uint16 VmpIdx;
  uint16 SubscListIdx;

  /* Sanity check area. Built to avoid further problems...                    */
  if(VarAddress == NULL)
    {
    /* The pointer is pointing to a non valid data memory position            */
    SubscProcess = ERR_PARAM_DATA;
    }
  else if(Callback == NULL)
    {
    /* The pointer is pointing to a non valid function memory position        */
    SubscProcess = ERR_PARAM_COMMAND;
    }
  else
    {
    /* If there are no problems look for the VARMAP index related to the      */
    /* variable                                                               */
    InternRet = VUR_GetVarmapIndex( VarAddress,
                                    &MapKind,
                                    &VmpIdx);

    /* Check the result gotten at the lookup process                          */
    if(InternRet == ANSWERED_REQUEST)
      {
      /* The address of the variable was found into the VARMAP                */
      SubscListIdx = 0;
      SubscProcess = OPERATION_RUNNING;
      do{
        if(SubscrList[SubscListIdx].Callback == NULL)
          {
          /* An empty position was found at the subscription list             */
          SubscProcess = ANSWERED_REQUEST;
          }
        else if(++SubscListIdx >= SUBSCR_LIST_SIZE)
          {
          /* The subscription list was fully checked and there is no empty    */
          /* positions where a new subscription could be stored               */
          SubscProcess = ERR_QFULL;
          DO_ONLY_ONCE( __DEBUGHALT(); );
          }
        }while(SubscProcess == OPERATION_RUNNING);

      if(SubscProcess == ANSWERED_REQUEST)
        {
        /* If the lookup process was concluded successfully, fill the         */
        /* parameters from the subscription list                              */
        if(Ticket != NULL)
          {
          *Ticket = SubscListIdx;
          }
        SubscrList[SubscListIdx].Callback = Callback;
        SubscrList[SubscListIdx].Parameter = Parameter;
        SubscrList[SubscListIdx].VmpIdx = VmpIdx;
        }
      }
    else
      {
      /* The address of the variable was not found into the VARMAP            */
      SubscProcess = ERR_PARAM_DATA;
      }
    }
  /* Return the function status result                                        */
  return SubscProcess;
  }




/* -----------------------------------------------------------------------------
VUR_Unsubscribe()
        Cancel the subscribe stored at the subscription list which is at the
        index sent by the parameter 'Ticket'.
--------------------------------------------------------------------------------
Input:  Ticket
          Index of the subscription which will be cancelled

Return: ANSWERED_REQUEST
          Process completed successfully
        OPERATION_IDLE
          There is no subscription stored at the index sent as parameter
        ERR_PARAM_RANGE
          The value of the index is out of range
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
ReturnCode_t VUR_Unsubscribe( uint32 Ticket )
  {
  ReturnCode_t UnsubscProcess;

  /* Sanity checks section                                                    */
  if(Ticket >= SUBSCR_LIST_SIZE)
    {
    /* If the index is out of the valid range */
    UnsubscProcess = ERR_PARAM_RANGE;
    }
  else if(SubscrList[Ticket].Callback == NULL)
    {
    /* If the index sent as target already is empty, so there is not to do    */
    UnsubscProcess = OPERATION_IDLE;
    }
  else
    {
    /* Subscription process                                                   */
    SubscrList[Ticket].Callback  = NULL;
    SubscrList[Ticket].Parameter = NULL;
    SubscrList[Ticket].VmpIdx = 0;
    UnsubscProcess = ANSWERED_REQUEST;
    }

  /* Return the function status result                                        */
  return UnsubscProcess;
  }




/* -----------------------------------------------------------------------------
VUR_Unsubscribe()
        Informs the VUR logic that a config variable have been changed by
          internal logic. VUR will check if there is a subscription to this
          variable and if so then it'll call the subscribed callbacks.
--------------------------------------------------------------------------------
Input:  VarAddress
          Address from the config variable that has been changed.

Return: ANSWERED_REQUEST
          Process completed successfully
        OPERATION_IDLE
          There is no subscription stored to the given variable
        ERR_PARAM_RANGE
          The value of the index is out of range
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
ReturnCode_t VUR_FlagVariableWrite( void *VarAddress )
{
  ReturnCode_t ReturnValue;

  if( VarAddress == NULL )
  {
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  {
    uint16       VarMapIndex;
    VarmapKind_t ConfigMap = MapConfig;

    /* Start by flagging that no subscription was found. If it will then the  */
    /*  answer will be modified.                                              */
    ReturnValue = OPERATION_IDLE;

    if( VUR_GetVarmapIndex( VarAddress, &ConfigMap, &VarMapIndex ) == ANSWERED_REQUEST )
    {
      /* If a subscription is triggered then change the return. Inform the    */
      /*  highest ID value to indicate an inner event.                        */
      if( VUR_TriggerSubscriptionsFor( VarMapIndex, (0 - 1) ) == ANSWERED_REQUEST )
      {
        ReturnValue = ANSWERED_REQUEST;
      }
    }
  }

  return ReturnValue;
}

/* *****************************************************************************
 *        PRIVATE FUNCTIONS AREA
 **************************************************************************** */
/* -----------------------------------------------------------------------------
VUR_PrepareData()
        This routine executes the necessary data treatment necessary to fit the
         input data into the VUR table.
        If the data size is smaller than the default (32 bits) than an appending
         has to be made. If it has 32 bits, though, then a memcpy operation is
         enough.
--------------------------------------------------------------------------------
Input:  Data
        DataSize

Return: Treated data
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
static uint32 VUR_PrepareData(void * Data, uint8 DataSize)
{
  uint32 DataResult;

  if( DataSize == 1 ) //8 bits
  {
    uint8  Data_8bits;
    memcpy( &Data_8bits, Data, 1);
    DataResult = Data_8bits;
  }
  else if( DataSize == 2 )  //16 bits
  {
    uint16 Data_16bits;
    memcpy( &Data_16bits, Data, 2);
    DataResult = Data_16bits;
  }
  else  //Data size = 4 or else
  { /* It is not expected data bigger than 4 bits */
    memcpy( &DataResult, Data, 4);
  }

  return DataResult;
}

/* -----------------------------------------------------------------------------
VUR_TriggerSubscriptionsFor()
        This routine will search for subscriptions for the given Output VarMap
          and will trigger their callbacks.
--------------------------------------------------------------------------------
Input:  VarMapIndex
        ID

Return: ANSWERED_REQUEST - At least one subscription has been triggered.
        Else             - No operation has been performed.
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
static ReturnCode_t VUR_TriggerSubscriptionsFor( uint16 VarMapIndex, uint8 ID )
{
  ReturnCode_t Result = OPERATION_IDLE;
  uint32       i = 0;

  for( i = 0; i < SUBSCR_LIST_SIZE; i++)
  {
    if( ( SubscrList[i].VmpIdx == VarMapIndex ) &&
        ( SubscrList[i].Callback != NULL      ) )
    {

      /* Call the function pointed by the stored pointer. Continue on the     */
      /*  loop because there may have more subscribers for this variable.     */
      (*SubscrList[i].Callback)(ID,SubscrList[i].Parameter);

      Result = ANSWERED_REQUEST;
    }
  }

  return Result;
}
