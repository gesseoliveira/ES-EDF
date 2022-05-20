/* *****************************************************************************
 FILE_NAME:     Controls.c
 DESCRIPTION:   Software library with controls implementations
 DESIGNER:      Juliano Varasquim
 CREATION_DATE: 01/ago/2016
 VERSION:       1.1
********************************************************************************
Version 1.0:    01/ago/2016 - Juliano Varasquim
                - First version of the source code
Version 1.1:    23/ago/2017 - Juliano Varasquim
                - Fixed issue where a reminiscent integration error would be
                    kept present if the kI parameter is changed.
***************************************************************************** */




/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
***************************************************************************** */
#include "Controls.h"
#include <math.h>




/* *****************************************************************************
 *
 *        DEFINES, ENUMS, STRUCTURES
 *
 **************************************************************************** */





/* *****************************************************************************
 *
 *        LOCAL PROTOTYPES
 *
***************************************************************************** */





/* *****************************************************************************
 *
 *        FUNCTIONS AREA
 *
***************************************************************************** */




/* -----------------------------------------------------------------------------
RunPID()
        Calculates the phase difference between the sine waves
--------------------------------------------------------------------------------
Input:  PID SetPoint
        PID kP
        PID kI
        PID kD
        PID MaxDeviat
        PID CyclesToChange
Output: NA
Return:
        value of calibration
--------------------------------------------------------------------------------
Notes:
----------------------------------------------------------------------------- */
float RunPID(ParamPID_t * Control, float Input)
  {
  #define SetPoint                      (*(Control->pSetPoint))
  #define kP                            (*(Control->pkP))
  #define kI                            (*(Control->pkI))
  #define kD                            (*(Control->pkD))
  #define MinOut                        (Control->MinOutPID)
  #define MaxOut                        (Control->MaxOutPID)
  #define Integral                      (Control->PrevIntegral)
  #define PrevInput                     (Control->PrevInput)
  #define Error                         (Control->ControlError)
  #define PrevkI                        (Control->PrevkI)

  float OutPID;
  float Derivative;


  /* Calculates the output error related to the set point                     */
  Error = SetPoint - Input;

  /* If the kI value changed then clear the integral component.               */
  if( PrevkI != kI )
    {
    PrevkI = kI;
    Integral = 0;
    }

  /* Updates the integral factor and saturate it between the OUTPUT limits    */
  Integral += ( kI * Error );
  if(Integral > MaxOut)
    {
    Integral = MaxOut;
    }
  else if(Integral < MinOut)
    {
    Integral = MinOut;
    }


  /* Update the derivative factor related to previous iteration               */
  Derivative = (Input - PrevInput);
  PrevInput = Input;


  /* Calculates the output value based on the P.I.D. factors and saturate it  */
  OutPID  = (kP * Error)
          + Integral
          - (kD * Derivative);

  if(OutPID > MaxOut)
    {
    OutPID = MaxOut;
    }
  else if(OutPID < MinOut)
    {
    OutPID = MinOut;
    }

  return OutPID;
  }




/* *****************************************************************************
 *
 *        LOCAL FUNCTIONS AREA
 *
***************************************************************************** */
