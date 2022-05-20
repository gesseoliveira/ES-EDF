/* *****************************************************************************
 FILE_NAME:     SortData.h
 DESCRIPTION:   Sort table data header.
 DESIGNER:      Denis Beraldo
 CREATION_DATE: 28/09/2016
 VERSION:       1.0
***************************************************************************** */
#ifndef SORT_DATA_H_
#define SORT_DATA_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "returncode.h"
#include "types.h"

/******************************************************************************/
/*              FIRMWARE VERSION                                              */
/******************************************************************************/
#define COMMON_SORTDATA_VER_MAJOR   1
#define COMMON_SORTDATA_VER_MINOR   0
#define COMMON_BRANCH_MASTER

/******************************************************************************/
/*              MACRO SORT INPUT PARAMETERS                                   */
/******************************************************************************/
/*
    struct_name - previous declared struct name (SortData_t type)
    tab_name    - sort table name
    table_type  - type of declared table
    e_type      - type of element to be sorted  (VarType_t)

    **** Optional parameter ****
    s_column    - column to sort (field name)
 */
#define F_4_PARAMS(struct_name, tab_name, table_type, e_type)                  \
   struct_name.table_addr   = (void *)&tab_name;                               \
   struct_name.row_amount   = (uint32_t)(sizeof(tab_name)/sizeof(table_type)); \
   struct_name.row_length   = (uint32_t)sizeof(table_type);                    \
   struct_name.sort_column  = NULL;                                            \
   struct_name.element_type = (VarType_t)e_type;                               \

#define F_5_PARAMS(struct_name, tab_name, table_type, e_type, s_column)        \
   struct_name.table_addr   = (void *)&tab_name;                               \
   struct_name.row_amount   = (uint32_t)(sizeof(tab_name)/sizeof(table_type)); \
   struct_name.row_length   = (uint32_t)sizeof(table_type);                    \
   struct_name.sort_column  = (void *)&tab_name[0].s_column;                   \
   struct_name.element_type = (VarType_t)e_type;                               \

#define P_ERROR     "Invalid parameter"

#define GET_MACRO(_1,_2,_3,_4,_5,NAME,...) NAME
#define FILL_SORT_PARAMS(...)   \
   GET_MACRO(__VA_ARGS__, F_5_PARAMS, F_4_PARAMS, P_ERROR, P_ERROR)(__VA_ARGS__)

/******************************************************************************/
/*              SORT INPUT PARAMETERS                                         */
/******************************************************************************/
typedef struct
{
    /* Pointer to table address.                                              */
    void *      table_addr;

    /* Number of rows in the table.                                           */
    uint32_t    row_amount;

    /* Table declared size. (length of each line in the table)                */
    uint32_t    row_length;

    /* Chosen column to sort. (use 'NULL' to 1-column table size)             */
    void *      sort_column;

    /* Element type. (used to correct type comparison)                        */
    VarType_t   element_type;

} SortData_t;

/******************************************************************************/
/*              INTERNAL DEFINITION                                           */
/******************************************************************************/
typedef struct
{
    VarType_t   varType;
    uint32_t    columnOffset;
} CompareConfig_t;


/******************************************************************************/
/*              INTERNAL PROTOTYPES                                           */
/******************************************************************************/
int _internal_s8Compare(const void *a, const void *b);
int _internal_u8Compare(const void *a, const void *b);
int _internal_s16Compare(const void *a, const void *b);
int _internal_u16Compare(const void *a, const void *b);
int _internal_s32Compare(const void *a, const void *b);
int _internal_u32Compare(const void *a, const void *b);
int _internal_floatCompare(const void *a, const void *b);
int _internal_stringCompare(const void *a, const void *b);

/******************************************************************************/
/*              EXTERNAL PROTOTYPE                                            */
/******************************************************************************/
ReturnCode_t SortData(SortData_t *SortDataInp);

#endif
