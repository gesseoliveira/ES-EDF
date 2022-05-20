/* *****************************************************************************
 FILE_NAME:     types.h
 DESCRIPTION:   This header file declares all the basic data types, along with
                  their limits. It also declares some Metroval's customized
                  data types.
 DESIGNER:      ???
 CREATION_DATE: ???
***************************************************************************** */

#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

/* *****************************************************************************
 *        INCLUDES
***************************************************************************** */
#include <stdint.h>
#include <limits.h>
#include "StdDebug.h"

/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define COMMON_TYPES_VER_MAJOR    1
#define COMMON_TYPES_VER_MINOR    2
#define COMMON_BRANCH_MASTER

/* *****************************************************************************
 *        DATA TYPES DECLARATION
***************************************************************************** */
/* Boolean value FALSE. It is defined always as a zero value.                 */
#ifndef FALSE
  #define  FALSE  0x00u
#endif


/* Boolean value TRUE. It is defined always as a non zero value.              */
#ifndef TRUE
  #define  TRUE   0x01u
#endif


/* Definition of value which will be used as an initializer to pointers.      */
#ifndef NULL
  #define  NULL   0x00u
#endif




/* Basic type definitions, created only to keep the compatibility with older  */
/* version of code. The use of those types must be avoided if possible.       */
#ifndef __cplusplus
  #ifndef bool
    typedef unsigned char       bool;
  #endif
#endif
typedef unsigned char           byte;
typedef unsigned short          word;
typedef unsigned long           dword;
typedef unsigned long long      dlong;




/* Preferred types definitions, created to be used as standard when an        */
/* integer value needs to be declared.                                        */
typedef signed char             int8;
typedef signed short int        int16;
typedef signed long int         int32;
typedef signed long long int    int64;

typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned long int       uint32;
typedef unsigned long long int  uint64;



/* Union type section. The types declared at this region are mainly used to   */
/* facilitate the data change between different types of data.                */
typedef union
  {
  uint64 asUint64;
  struct
    {
    uint32 Lo;
    uint32 Hi;
    } asUint32;
  } UINT64xUINT32;

typedef union
  {
  uint64 asUint64;
  struct
    {
    uint32 Lo;
    uint32 Hi;
    } asUint32;
  struct
    {
    uint16 LoLo;
    uint16 Lo;
    uint16 Hi;
    uint16 HiHi;
    } asUint16;
  } UINT64xUINT32xUINT16;

typedef union
  {
  uint32 asUint32;
  struct
    {
    uint16 Lo;
    uint16 Hi;
    } asUint16;
  } UINT32xUINT16;

typedef union
  {
  uint16 asUint16;
  struct
    {
    uint8 Lo;
    uint8 Hi;
    } asUint8;
  } UINT16xUINT8;


/* Enumeration used to represent a type when the type needs to be stored in a */
/* table, or tested in the program as for example at a switch-case.           */
typedef enum
  {
  INT8,
  UINT8,
  INT16,
  UINT16,
  INT32,
  UINT32,
  FLOAT,
  INT64,
  UINT64,
  DATE_TYPE,
  TIME_TYPE,
  STRING = 0x80,
  } VarType_t;




/* Enumeration used to represent the access level aligned with the definition */
  /* made about the "Metroval Standard".                                      */
typedef enum
  {
  BASIC               = 0x0000,
  ADVAN,
  ENGIN,
  MAINT,
  MANUF,
  INVALID_USER_LEVEL  = 0xFFFE,
  NULL_USER_LEVEL     = 0xFFFF,
  } UserLevel_t;




/* Enumerations used to set the behavior of the variables into the VARMAP.    */
typedef enum
  {
  VOLATILE          = 0x00,
  NON_VOLATILE      = 0x01,
  EXTERNAL_VAR      = 0x02,
  } Volatility_t;
typedef enum
  {
  IGNORE_CHANGES = 0,
  LOG_CHANGES,
  } LogChanges_t;


/* Enumeration used to define the diagnos variable status according to its    */
/*  thresholds                                                                */
typedef enum VarStatus_e
  {
  RegularValue      = 0x00,
  LowValue          = 0x02,
  TooLowValue       = 0x03,
  HighValue         = 0x04,
  TooHighValue      = 0x0C,
  InvalidValue      = 0x0F,
  VarStatusSizeDef  = 0xFFFF,
  } VarStatus_t;




/* Considering that one meter has 2 flow connections (A and B), then one can  */
/* say that the flow can occur in two direction i.e. from A to B and from B   */
/* to A. It means the totalization may be performed of 4 manners:             */
/*  - Considering only the flow from A to B                                   */
/*  - Considering only the flow from B to A                                   */
/*  - Considering the flow from A to B as Positive and the one from B to A as */
/*    Negative                                                                */
/*  - Considering the flow from B to A as Positive and the one from A to B as */
/*    Negative                                                                */
typedef enum MetersTotMode_e
  {
  ModeTotOnlyAB = 0,
  ModeTotOnlyBA,
  ModeTotABminusBA,
  ModeTotBAminusAB,

  ModeTotInvalid,   /* Warning, this item must be kept immediately after the  */
                    /* valid modes, so its position should never be changed   */
  ForceMetersTotModeAs16Bits = 0xFFFF,
  } MetersTotMode_t;




/* Structure containing the configuration fields used by the standard memory  */
/*  interface                                                                 */
typedef struct
{
  bool   isAbsoluteAddressing;   /* TRUE = Absolute ; FALSE = Relative        */
  uint32 StartAddress;           /* Used only if isAbsoluteAddressing != FALSE*/
  uint32 FileSize;
  uint32 RecordSize;
  char   *FileName;
} MEMPar_t;


/* Memory state definition.                                                   */
typedef enum
{
  INITIALIZING_MEMORY,
  MEMORY_IS_PRESENT,
  MEMORY_IS_NOT_PRESENT,
  MEMORY_ENUM_SIZE        = 0xFFFF,
} MEMState;

/* Structure memory returned by get status memory interface.                  */
typedef struct
{
  MEMState  MemoryState;
  uint32    TotalSpace;
  uint32    SpaceAvailable;
} MEMStatus_t;

/* Array of pointers */
typedef struct
{
  void    *Ptr_to_Var;
  uint8   Size_of_Var;
} __attribute__ ((packed)) Prt_Table_st;

/* TimeBCD definition : 0xHHMMSS00                                            */
typedef union
{
  struct
  {
    uint8 Padding;  /* 0x00        */
    uint8 Secs;     /* 0x00 - 0x59 */
    uint8 Mins;     /* 0x00 - 0x59 */
    uint8 Hours;    /* 0x00 - 0x23 */
  };
  uint32 Value;     /* 0xHHMMSS00  */
} TimeBDC_t;

/* DateBCD definition : 0xDDMMAAAA                                            */
typedef union
{
  struct
  {
    uint16 Years;   /* 0x2000 - 0x2099 */
    uint8  Months;  /*   0x01 -   0x12 */
    uint8  Days;    /*   0x01 -   0x31 */
  };
  uint32 Value;     /* 0xDDMMAAAA      */
} DateBDC_t;

/* Time Struct definition:                                                    */
typedef struct
{
  uint8 Secs;       /*   seconds (0 - 59) */
  uint8 Mins;       /*   minutes (0 - 59) */
  uint8 Hours;      /*   hours   (0 - 23) */
} Time_t;

/* Date Struct definition:                                                    */
typedef struct
{
  uint8  Days;      /*   day   (1 - 31) */
  uint8  Months;    /*   month (1 - 12) */
  uint16 Years;     /*   year           */
} Date_t;

/* DateTime Struct definition                                                 */
typedef struct
{
  Time_t TimeVal;
  Date_t DateVal;
} DateTime_t;
#define DATETIME_INIT   { { 0, 0, 0 }, { 0, 0, 0 } }

/* Timestamp type definition                                                  */
typedef uint32  TimeStamp_t;

/* Network IP definition                                                      */
typedef union
{
  uint32 Long;
  uint8  Byte[4];
} IPv4_t;

/**********************************************************/
/* Uniform multiplatform 8-bits peripheral access macros */
/**********************************************************/

/* Enable maskable interrupts */
#define __EI()\
 do {\
  /*lint -save  -e950 Disable MISRA rule (1.1) checking. */\
     __asm("CPSIE i");\
  /*lint -restore Enable MISRA rule (1.1) checking. */\
 } while(0)

/* Disable maskable interrupts */
#define __DI() \
 do {\
  /*lint -save  -e950 Disable MISRA rule (1.1) checking. */\
     __asm ("CPSID i");\
  /*lint -restore Enable MISRA rule (1.1) checking. */\
 } while(0)



/* Save status register and disable interrupts */
#define EnterCritical() \
 do {\
  uint8_t SR_reg_local;\
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
   __asm ( \
   "MRS R0, PRIMASK\n\t" \
   "CPSID i\n\t"            \
   "STRB R0, %[output]"  \
   : [output] "=m" (SR_reg_local)\
   :: "r0");\
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */\
   if (++SR_lock == 1u) {\
     SR_reg = SR_reg_local;\
   }\
 } while(0)


/* Restore status register  */
#define ExitCritical() \
 do {\
   if (--SR_lock == 0u) { \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
     __asm (                 \
       "ldrb r0, %[input]\n\t"\
       "msr PRIMASK,r0;\n\t" \
       ::[input] "m" (SR_reg)  \
       : "r0");                \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */\
   }\
 } while(0)

#define ___NOP() \
  /*lint -save  -e586 -e950 Disable MISRA rule (2.1,1.1) checking. */\
  __asm( "NOP") \
  /*lint -restore Enable MISRA rule (2.1,1.1) checking. */


#endif
