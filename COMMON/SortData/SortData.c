/* *****************************************************************************
 FILE_NAME:     SortData.c
 DESCRIPTION:   Sort table data routines.
 DESIGNER:      Denis Beraldo
 CREATION_DATE: 28/09/2016
 VERSION:       1.0
********************************************************************************
Version 1.0: 28/09/2016 - Denis Beraldo
            - Initial version
***************************************************************************** */

#include "SortData.h"

/*
 * Public variable
 * Store configurations needed by '_int_GenericCompare' function.
 */
static CompareConfig_t CompareConfig;

/*
 * Sort data function
 *  Sort table pointed by 'table_addr'.
 *  The result is placed at 'table_addr'.
 *
 * Input: SortDataInp - structure with input parameters.
 *          (see 'SortData_t' for more informations)
 *
 * Return:  - ANSWERED_REQUEST for OK.
 *          - ERR_FAILED for parameter error.
 *
 */
ReturnCode_t SortData(SortData_t *SortDataInp)
{
    ReturnCode_t ret;
    uint32_t tempOffset;

    /* Parameter check */
    if ( (SortDataInp->table_addr != NULL)          &&
           (SortDataInp->row_amount > 0)            &&
           (SortDataInp->row_length > 0)            &&
           (SortDataInp->element_type <= STRING) )
    {
        /* Save variable type for future use. */
        CompareConfig.varType = SortDataInp->element_type;

        /* Default offset value. */
        CompareConfig.columnOffset = 0;

        /* Calculate column offset. */
        if (SortDataInp->sort_column != NULL)
        {
            /* Temporary offset. */
            tempOffset = (uint32_t)(SortDataInp->sort_column - SortDataInp->table_addr);

            /* Check if tempOffset has a valid range. */
            if (tempOffset < (uint32_t)SortDataInp->row_length)
            {
                CompareConfig.columnOffset = tempOffset;
            }
        }

        /* Until now, answered request is the return. */
        ret = ANSWERED_REQUEST;

        /* Choose comparison method according to variable type. */
        switch (CompareConfig.varType)
        {
            case INT8:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_s8Compare);
            }
            break;

            case UINT8:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_u8Compare);
            }
            break;

            case INT16:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_s16Compare);
            }
            break;

            case UINT16:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_u16Compare);
            }
            break;

            case INT32:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_s32Compare);
            }
            break;

            case UINT32:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_u32Compare);
            }
            break;

            case FLOAT:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_floatCompare);
            }
            break;

            case STRING:
            {
                /* Call sort function with specific compare function. */
                qsort(SortDataInp->table_addr, (size_t)SortDataInp->row_amount, (size_t)SortDataInp->row_length, _internal_stringCompare);
            }
            break;

            /* Not prepared to treat this data types comparison. */
            case INT64:
            case UINT64:
            case DATE_TYPE:
            case TIME_TYPE:
            {
                /* Parameter error. */
                ret = ERR_PARAM_RANGE;
            }
            break;
        }
    }
    else
    {
        /* Parameter error. */
        ret = ERR_PARAM_RANGE;
    }

    return ret;
}

/*
 * Compare functions - internal use.
 *
 * This function is called by 'qsort'.
 * The function prototype must follow this type:
 *    int (*__compar_fn_t) (__const void *, __const void *)
 *
 * Input: variables to be compared
 *
 * Returns: 0 - 'a' and 'b' are equal
 *          1 - 'a' is greater than 'b'
 *         -1 - 'a' is less than 'b'
 *
 * TODO: Need more tests with signed/unsigned 8-bits and 16-bits.
 *
 */
int _internal_s8Compare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const int8_t *)a > *(const int8_t *)b) -
            (*(const int8_t *)a < *(const int8_t *)b) );

    return ret;
}

int _internal_u8Compare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const uint8_t *)a > *(const uint8_t *)b) -
            (*(const uint8_t *)a < *(const uint8_t *)b) );

    return ret;
}

int _internal_s16Compare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const int16_t *)a > *(const int16_t *)b) -
            (*(const int16_t *)a < *(const int16_t *)b) );

    return ret;
}

int _internal_u16Compare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const uint16_t *)a > *(const uint16_t *)b) -
            (*(const uint16_t *)a < *(const uint16_t *)b) );

    return ret;
}

int _internal_s32Compare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const int32_t *)a > *(const int32_t *)b) -
            (*(const int32_t *)a < *(const int32_t *)b) );

    return ret;
}

int _internal_u32Compare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const uint32_t *)a > *(const uint32_t *)b) -
            (*(const uint32_t *)a < *(const uint32_t *)b) );

    return ret;
}

int _internal_floatCompare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = ( (*(const float *)a > *(const float *)b) -
            (*(const float *)a < *(const float *)b) );

    return ret;
}

int _internal_stringCompare(const void *a, const void *b)
{
    /* Return */
    int ret = 0;

    /* Add column offset. */
    a += CompareConfig.columnOffset;
    b += CompareConfig.columnOffset;

    ret = strcmp(*(const char **)a, *(const char **)b);

    return ret;
}
