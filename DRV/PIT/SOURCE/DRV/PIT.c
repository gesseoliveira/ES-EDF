/**
 ******************************************************************************
 * @file    PIT.c
 * @author  Plínio Barbosa da Silva
 * @brief   Periodic Interrupt Timer module driver.
 * @brief   Version 2019.7.3
 * @brief   Creation Date 02/04/2019
 *
 *          This file provides functions to manage software timers
 *          functionalities:
 *           + Initialization of a software timer and monitoring timer
 *           + Peripheral Control functions
 *
 *  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================

  ...

    // Create a timer event of 100ms
  (void)Set_Timer(0, 100, MiliSec);
  // Create a timer event of 500ms
  (void)Set_Timer(1, 500, MiliSec);


  while (1)
  {
    if(Get_Timer(0) == ANSWERED_REQUEST){ // Verif
      (void)Set_Timer(TIMER_LED1, 100, MiliSec); // Restart the timer for the led1
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
    }

    if(Get_Timer(1) == ANSWERED_REQUEST){
      (void)Set_Timer(TIMER_LED2, 500, MiliSec); // Restart the timer for the led2
      HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
    }
  }

  ...

  @endverbatim
 */

/*_______Include List______*/
#include "PIT.h"
#include "returncode.h"
#include "types.h"
#include "sys_cfg_stm32f407.h"
/*_______End Include List____*/

uint32_t DRV_PIT_Tick_Counter;

/* Private macros --------------------------------------------------------*/
/** @defgroup PIT_Private_Macros PIT Private Macros
 * @{
 */

/**
 * Return the actual tick counter.
 */
#define GetPITCount()         (uint32_t)Sys_Get_Tick() /* PIT Peripheral counts backwards */

/**
 * Determine the difference between the maximum count and actual counter value.
 */
#define GetCountToOverflow()  (uint32_t)(0xffffffff - GetPITCount())  /* PIT Peripheral counts backwards */

/**
 * @}
 */

/**
 * PIT configuration structures.
 */
PIT_TimerParam  Timer[PIT_MAX_TIMERS_AVAILABLE];

/**
 * PIT configuration flag.
 */
bool            isPeripheralConfigured = FALSE;

/**
 * PIT Enabled flag.
 */
bool            isPeripheralEnabled = FALSE;

/**
 * @brief  Configure timer to generate periodic interrupt time.
 */
void PIT_ConfigurePeripheral(void)
{
}

/**
 * @brief  Start time counting routine.
 * @param  ID : Requester's ID
 * @param  CountsToPerform : How many ticks to count
 * @param  Unit : Tick's time base.
 *         This parameter can be one of the following values:
 *             @arg MiliSec: Counter is set to miliseconds base.
 * @retval Result : Result of Operation
 *         This parameter can be one of the following values:
 *              @arg ANSWERED_REQUEST: All ok, time is being counted
 *              @arg Else: Some error happened.
 */

ReturnCode_t Set_Timer(uint8_t ID, uint32_t CountsToPerform, ValidUnitsOfPIT Unit)
{
  ReturnCode_t ReturnValue;
  uint32_t CountsToOverflow;
  uint32_t CurrentPitCount;


  /* First, perform some sanity check. ID must be lower than limit, counts    */
  /*  must be different than zero and Unit must be of Milissecond ( no other  */
  /*  is implemented).                                                        */
  if( ( ID >= PIT_MAX_TIMERS_AVAILABLE) || ( CountsToPerform == 0 ) || ( Unit != MiliSec ) )
  { /* Wrong parameter(s) */
    ReturnValue = ERR_PARAM_RANGE;
  }
  /* Check if the device is disabled. If it is, it cannot perform the count.  */
  else if( ( isPeripheralEnabled == FALSE ) && ( isPeripheralConfigured != FALSE ) )
  { /* Device is disabled, cannot proceed */
    ReturnValue = ERR_DISABLED;
  }

  else
  {
    /* If the peripheral is not yet configured, do so now. (first time)       */
    if(isPeripheralConfigured == FALSE)
    {
      isPeripheralConfigured = TRUE;  /* Peripheral will now be configured.   */
      isPeripheralEnabled = TRUE;     /* Peripheral will start enabled.       */

      PIT_ConfigurePeripheral();      /* Call configuration routine.          */
    }

    /* Calculate if the given time will happen after a timer overflow event   */
    CurrentPitCount = GetPITCount();

    CountsToOverflow = GetCountToOverflow();

    if(CountsToOverflow >= CountsToPerform)
    { /* Count will finish before the overflow event */
      Timer[ID].CountLimit = CurrentPitCount + CountsToPerform;
      Timer[ID].WaitOverflow = FALSE;
    }
    else
    { /* Count will finish after the overflow event */
      Timer[ID].CountLimit = CountsToPerform - CountsToOverflow;
      Timer[ID].WaitOverflow = TRUE;
    }

    /* Define status */
    Timer[ID].Status = OPERATION_RUNNING;

    /* Saves the count value at request */
    Timer[ID].CountAtRequest = CurrentPitCount;

    /* Job done */
    ReturnValue = ANSWERED_REQUEST;

  }

  return ReturnValue;
}

/**
 * @brief  Routine that checks if the requested time has passed.
 * @param  ID : Requester's ID
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *          @arg ANSWERED_REQUEST: All ok, time is being counted
 *          @arg OPERATION_RUNNING: Time has not passed yet, keep waiting.
 *          @arg Else: Some error happened.
 */
ReturnCode_t Get_Timer(uint8_t ID)
{
  ReturnCode_t ReturnValue;
  uint32_t CurrentPitCount;

  if (ID >= PIT_MAX_TIMERS_AVAILABLE)
  { /* Wrong parameter(s) */
    ReturnValue = ERR_PARAM_RANGE;
  }
  /* Check if the device is disabled. If it is, it cannot perform the count.  */
  else if( ( isPeripheralEnabled == FALSE ) && ( isPeripheralConfigured != FALSE ) )
  { /* Device is disabled, cannot proceed */
    ReturnValue = ERR_DISABLED;
  }
  else if (Timer[ID].Status == ANSWERED_REQUEST)
  { /* Timer has already expired. */
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Check current PIT value and calculate if it has expired.               */
    CurrentPitCount = GetPITCount();

    if (Timer[ID].WaitOverflow != FALSE)
    {
      if (CurrentPitCount < Timer[ID].CountAtRequest)
      {
        Timer[ID].WaitOverflow = FALSE;
      }
      ReturnValue = OPERATION_RUNNING;
    }
    else
    {
      if (CurrentPitCount >= Timer[ID].CountLimit)
      {
        Timer[ID].Status = ANSWERED_REQUEST;
        ReturnValue = ANSWERED_REQUEST;
      }
      else
      {
        ReturnValue = OPERATION_RUNNING;
      }
    }
  }
  return ReturnValue;
}

/**
 * @brief  Stop driver's operation.
 * @retval Result : Result of Operation
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg Else: Some error happened.
 */
ReturnCode_t Halt_Timer(void)
{
  /* Simply inform that process went well and mark that driver is disabled.   */
  isPeripheralEnabled = FALSE;

  /* Stop timer*/
  //  TIM3->CR1 &= ~TIM_CR1_CEN;

  return ANSWERED_REQUEST;
}

/**
 * @brief  Resume driver's operation.
 * @retval Result : Result of Operation
 *         This parameter can be one of the following values:
 *            @arg ANSWERED_REQUEST: All ok, time is being counted
 *            @arg Else: Some error happened.
 */
ReturnCode_t Resume_Timer(void)
{
  /* Simply inform that process went well and mark that driver is running.  */
  isPeripheralEnabled = TRUE;

  /*Start timer*/
  //  TIM3->CR1 |= TIM_CR1_CEN;

  return ANSWERED_REQUEST;
}

