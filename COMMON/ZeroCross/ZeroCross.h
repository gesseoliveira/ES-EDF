/* ************************************************************************************************************
 FILE_NAME:     ZeroCross.h
 DESCRIPTION:   Software library for zero crossing detection
 DESIGNER:      Felipe Zanoni
 CREATION_DATE: 27/sep/2018
 VERSION:       1.0
************************************************************************************************************ */
#ifndef ZEROCROSS_H_INCLUDED
#define ZEROCROSS_H_INCLUDED

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
#define COMMON_ZEROCROSS_VER_MAJOR  1
#define COMMON_ZEROCROSS_VER_MINOR  0
#define COMMON_BRANCH_MASTER

/* ************************************************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 ************************************************************************************************************ */

typedef struct {
  float    phaseDiff; //!< Phase difference between s1 and s2 in degrees
  float    freq;      //!< Signal frequency in Hz
  uint32_t sampleIdx; //!< Index of sample that zero crossing occurred
  float    ampWeiSum; //!< Peak to peak amplitude from weighted sum signal
  float    ampSign01; //!< Peak to peak amplitude from signal #1
  float    ampSign02; //!< Peak to peak amplitude from signal #2
  float    timeDiff;  //!< Time difference between s1 and s2 in seconds?
} ZeroCross_Result_Detail;

typedef struct {
  ZeroCross_Result_Detail data[3]; //!< Result for last 3 zero crossing
  char zcFoundCnt;                 //!< Zero crossing detection count
} ZeroCross_Results;


/* ************************************************************************************************************
 *
 *        PROTOTYPES
 *
************************************************************************************************************ */
void ZeroCross__Init(const float sample_freq);
void ZeroCross__Run(const float s1[], const float s2[], const float ws[], uint16_t samples, ZeroCross_Results *results);

#endif /* ZEROCROSS_H_INCLUDED */
