/* *****************************************************************************
 FILE_NAME:     WaveFit.h
 DESCRIPTION:   Software library with math functions used to perform wave 
                fitting
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 20/feb/2017
 VERSION:       1.0
***************************************************************************** */
#ifndef WAVEFIT_H_INCLUDED
#define WAVEFIT_H_INCLUDED




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "types.h"
#include "returncode.h"
#include "macros.h"
#include <math.h>
#include "SETUP.h"


/* *****************************************************************************
 *
 *        FIRMWARE VERSION
 *
***************************************************************************** */
#define COMMON_WAVEFIT_VER_MAJOR  1
#define COMMON_WAVEFIT_VER_MINOR  0
#define COMMON_WAVEFIT_BRANCH_MASTER




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */
#ifndef MAX_POLYFIT_LEN
  #define MAX_POLYFIT_LEN 50
#endif

#ifndef MAX_POLYFIT_ORD
  #define MAX_POLYFIT_ORD 6
#endif

/* *****************************************************************************
 *
 *        PROTOTYPES
 *
***************************************************************************** */
ReturnCode_t PolyFit(float xDat[], float yDat[], int32 DatLen, int32 PolOrd, float fDat[], float *pR2, float pCoef[]);




#endif /* WAVEFIT_H_INCLUDED */
