/* *****************************************************************************
 FILE_NAME:     Controls.h
 DESCRIPTION:   Software library with controls implementations
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 01/ago/2016
 VERSION:       1.1
***************************************************************************** */
#ifndef CONTROLS_H_INCLUDED
#define CONTROLS_H_INCLUDED




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "Types.h"
#include "ReturnCode.h"
#include "Macros.h"
#include "setup.h"




/* *****************************************************************************
 *
 *        FIRMWARE VERSION
 *
***************************************************************************** */
#define COMMON_CONTROLS_VER_MAJOR   1
#define COMMON_CONTROLS_VER_MINOR   1
#define COMMON_BRANCH_MASTER




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */

#define HIDDEN_PID_INIT_VALUES                    0.0F,0.0F,0.0F,0.0F

/* PID control parameters structure                                           */
typedef struct
  {
  float *pSetPoint;
  float *pkP;
  float *pkI;
  float *pkD;
  float MinOutPID;
  float MaxOutPID;
  float PrevIntegral;
  float PrevInput;
  float ControlError;
  float PrevkI;
  } ParamPID_t;



/* ************************************************************************************************************
 *
 *        PROTOTYPES
 *
************************************************************************************************************ */
  float RunPID(ParamPID_t * control, float Input);




#endif /* CONTROLS_H_INCLUDED */
