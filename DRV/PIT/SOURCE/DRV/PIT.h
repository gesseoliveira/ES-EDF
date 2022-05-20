/**
 ******************************************************************************
 * @file    PIT.h
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

#ifndef __PIT_H__
#define __PIT_H__

#include "returncode.h"
#include "types.h"
#include "SETUP.h"
/**
 * Max number of PIT's.
 */
#ifndef PIT_MAX_TIMERS_AVAILABLE
#define PIT_MAX_TIMERS_AVAILABLE 10
#endif
/** @defgroup PIT_Structures PIT Types and Enums
 * @{
 */

/** @defgroup PIT_Units PIT counter units
 * @{
 */

/**
 * @brief  Timer units enumeration
 */
typedef enum
{
  MiliSec, /*!< Specifies the unit of milliseconds to the counter. */
} ValidUnitsOfPIT;
/**
 * @}
 */

/**
 * @brief  Software timer control structure.
 */
typedef struct
{
  ValidUnitsOfPIT UsedUnit;         /*!< Specifies the unit of the counter to be
                                    configured. This parameter can be any value
                                    of @ref PIT_Units                         */
  uint32_t          CountLimit;     /*!< Specifies the time to count.
                                    This parameter can be any uint32_t value  */
  uint32_t          CountAtRequest; /*!< Specifies the actual time count.     */
  bool              WaitOverflow;   /*!< Specifies if the time count is done. */
  uint32_t          Status;         /*!< Specifies the time to count.
                                    This parameter can be any uint32_t value  */
} PIT_TimerParam;

/**
 * @brief  Configure timer to generate periodic interrupt time.
 * @note   This routine will resume the PIT driver, making it able to do its
 *  operations again. It is usually called after a wake up event.
 */
void PIT_ConfigurePeripheral(void);

/**
 * @brief  Start time counting routine.
 * @param  ID : Requester's ID
 * @param  CountsToPerform : How many ticks to count
 * @param  Unit : Tick's time base.
 *         This parameter can be one of the following values:
 *             @arg MiliSec: Counter is set to miliseconds base.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg Else: Some error happened.
 * @note   This routine will start a time counting operation. If it was already
 *         counting time for this ID the previous operation will be aborted.
 *         Use the Get_Timer routine to check if the given period has passed.
 */
ReturnCode_t Set_Timer(uint8_t ID, uint32_t CountsToPerform, ValidUnitsOfPIT Unit);

/**
 * @brief  Routine that checks if the requested time has passed.
 * @param  ID : Requester's ID
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg OPERATION_RUNNING: Time has not passed yet, keep waiting.
 *             @arg Else: Some error happened.
 *
 */
ReturnCode_t Get_Timer(uint8_t ID);

/**
 * @brief  Stop driver's operation.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *            @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg Else: Some error happened.
 * @note   This routine will stop the PIT driver, making it not count the time
 * periods anymore. This is usually called before entering stop mode so
 * that the driver's interrupts doesn't wake up the device.
 */
ReturnCode_t Halt_Timer(void);

/**
 * @brief  Resume driver's operation.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg Else: Some error happened.
 * @note   This routine will resume the PIT driver, making it able to do its
 * operations again. It is usually called after a wake up event.
 */
ReturnCode_t Resume_Timer(void);

#endif
