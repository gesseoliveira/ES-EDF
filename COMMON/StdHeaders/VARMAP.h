/* *****************************************************************************
 FILE_NAME:     Varmap.h
 DESCRIPTION:   VARMAP Header File
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: oct/2015
 VERSION:       2.0
 *******************************************************************************
 Version 1.0:   15/10/2015 - Juliano Varasquim
                - First Version
 Version 2.0:   28/09/2016 - Juliano Varasquim
                - File moved to commom repository.
                - Changed macros' names and allowed declaration of matrices.
 **************************************************************************** */
#ifndef VARMAP_H_INCLUDED
#define VARMAP_H_INCLUDED


/* *****************************************************************************
 *        INCLUDES
 **************************************************************************** */
#include "Types.h"
#include "Macros.h"


/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define COMMON_VARMAP_VER_MAJOR		2
#define COMMON_VARMAP_VER_MINOR		0
#define COMMON_BRANCH_MASTER


/* *****************************************************************************
 *        DEFINES, ENUMS, STRUCTURES
***************************************************************************** */

/* Definition of the structure used to create the Config Table                */
typedef struct
  {
  void *        VarAddress;
  uint8         Length;
  VarType_t     Type;
  UserLevel_t   AccessLevel;
  Volatility_t  Volatility;
  LogChanges_t  LogChanges;
  uint16        ArrayLength;
  } ConfigTable_t;

/* Definition of the structure used to create the Values Table                */
typedef struct
  {
  void *        VarAddress;
  uint8         Length;
  VarType_t     Type;
  UserLevel_t   AccessLevel;
  Volatility_t  Volatility;
  uint16        ArrayLength;
  } OutputTable_t;

typedef struct
  {
  void *        VarAddress;
  UserLevel_t   AccessLevel;
  } ReadAccessTable_t;

/* Macro used to create the ConfigTable                                       */
#define CREATE_VARMAP_CONFIG_TABLE()                                           \
                                     const ConfigTable_t VarMapConfigTable[] =

/* Macros used to add new item in ConfigTable                                 */
#define ADD_SINGLE_CFG_ITEM( VAR, TYPE, LEVEL, VOLAT, LOG )       { &VAR.Current,           sizeof(VAR.Current),           TYPE, LEVEL, VOLAT, LOG,  1                                           }
#define ADD_MATRIX_CFG_ITEM( VAR, TYPE, LEVEL, VOLAT, LOG, I)     { &VAR.Current I ,        sizeof(VAR.Current I ),        TYPE, LEVEL, VOLAT, LOG,  sizeof(VAR.Current)/sizeof(VAR.Current I )  }

/* Macro used to finish the ConfigTable                                       */
#define FINISH_VARMAP_CONFIG_TABLE()                                           \
                                     ;                                         \
                                     const uint16 VarMapConfigLen = sizeof(VarMapConfigTable)/sizeof(ConfigTable_t)


/* Macro used to create the OutputTable                                       */
#define CREATE_VARMAP_OUTPUT_TABLE()                                           \
                                     const OutputTable_t VarMapOutputTable[] =

/* Macros used to add new item in OutputTable                                 */
#define ADD_SINGLE_OUT_ITEM( VAR, TYPE, LEVEL, VOLAT)             { &VAR.Value,             sizeof(VAR.Value),             TYPE, LEVEL, VOLAT,  1                                             }
#define ADD_MATRIX_OUT_ITEM( VAR, TYPE, LEVEL, VOLAT, I)          { &VAR.Value I ,          sizeof(VAR.Value I ),          TYPE, LEVEL, VOLAT,  sizeof(VAR.Value)/sizeof(VAR.Value I )        }
#define ADD_SINGLE_OUT_STRC( VAR, STRUCT, TYPE, LEVEL, VOLAT)     { &VAR.Value.STRUCT,      sizeof(VAR.Value.STRUCT),      TYPE, LEVEL, VOLAT,  1                                             }
#define ADD_MATRIX_OUT_STRC( VAR, STRUCT, TYPE, LEVEL, VOLAT, I)  { &VAR.Value I .STRUCT,   sizeof(VAR.Value I .STRUCT),   TYPE, LEVEL, VOLAT,  sizeof(VAR.Value)/sizeof(VAR.Value I .STRUCT) }

/* Macro used to finish the OutputTable                                       */
#define FINISH_VARMAP_OUTPUT_TABLE()                                           \
                                     ;                                         \
                                     const uint16 VarMapOutputLen = sizeof(VarMapOutputTable)/sizeof(OutputTable_t);

/* Macro used to create the ReadAccessTable                                    */
#define CREATE_VARMAP_READ_ACCESS_TABLE()                                      \
                                     const ReadAccessTable_t  VarMapReadAccessTable[] =

/* Macros used to add new item in ReadAccessTable                             */
#define ADD_SINGLE_CFG_ITEM_READ_ACCESS( VAR, LEVEL)              { &VAR.Current,           LEVEL }
#define ADD_MATRIX_CFG_ITEM_READ_ACCESS( VAR, LEVEL, I)           { &VAR.Current I,         LEVEL }

#define ADD_SINGLE_OUT_ITEM_READ_ACCESS( VAR, LEVEL)              { &VAR.Value,             LEVEL }
#define ADD_MATRIX_OUT_ITEM_READ_ACCESS( VAR, LEVEL, I)           { &VAR.Value I,           LEVEL }
#define ADD_SINGLE_OUT_STRC_READ_ACCESS( VAR, STRUCT, LEVEL)      { &VAR.Value.STRUCT,      LEVEL }
#define ADD_MATRIX_OUT_STRC_READ_ACCESS( VAR, STRUCT, LEVEL, I)   { &VAR.Value I .STRUCT,   LEVEL }

/* Macro used to finish the ReadAccessTable                                   */
#define FINISH_VARMAP_READ_ACCESS_TABLE()                                      \
                                     ;                                         \
                                     const uint16 VarMapReadAccessLen = sizeof(VarMapReadAccessTable)/sizeof(ReadAccessTable_t);

/* -- OBSOLETE MACROS. FOR COMPATIBILITY ONLY, SHOULD NOT BE USED ANYMORE! -- */
#define ADD_CONFIG_SNG_ITEM                                  ADD_SINGLE_CFG_ITEM
#define ADD_CONFIG_MAT_ITEM(VA,T,LV,VO,LO,I)                 ADD_MATRIX_CFG_ITEM(VA,T,LV,VO,LO,[I])
#define ADD_OUTPUT_SNG_ITEM                                  ADD_SINGLE_OUT_ITEM
#define ADD_OUTPUT_MAT_ITEM(VA,T,LV,VO,I)                    ADD_MATRIX_OUT_ITEM(VA,T,LV,VO,[I])
#define ADD_OUTPUT_SNG_STRC                                  ADD_SINGLE_OUT_STRC
#define ADD_OUTPUT_MAT_STRC(VA,S,T,LV,VO,I)                  ADD_MATRIX_OUT_STRC(VA,S,T,LV,VO,[I])
/* -------------------------------------------------------------------------- */


/* *****************************************************************************
 *        APPLICATION GLOBAL VARIABLES
***************************************************************************** */
extern const ConfigTable_t      VarMapConfigTable[];
extern const OutputTable_t      VarMapOutputTable[];
extern const ReadAccessTable_t  VarMapReadAccessTable[];
extern const uint16 VarMapConfigLen;
extern const uint16 VarMapOutputLen;
extern const uint16 VarMapReadAccessLen;

#endif
