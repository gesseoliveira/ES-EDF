/**
 ******************************************************************************
 * @file    QUAD.h
 * @author  Icaro Nascimento Queiroz
 * @brief   Quadrature module driver.
 * @brief   Version 2019.12.1
 * @brief   Creation Date 15/08/2019
 *
 *          This file provides functions to manage Encoder
 *          functionalities:
 *           + Initialization of a Timer as Encoder
 *           + Read pulses from Encoder
 *           + Get Frequency
 *           + Get Direction of spinning
 */

#ifndef QUAD_DRV_H
#define QUAD_DRV_H

/* *****************************************************************************
 *        INCLUDES
***************************************************************************** */
#include "types.h"
#include "string.h"
#include "returncode.h"
#include "macros.h"
#include "sys_cfg_stm32f407.h"
#include "SETUP.h"

/* *****************************************************************************
 *        FIRMWARE VERSION
***************************************************************************** */
#define DRV_QUAD_VER_MAJOR                                                  2019
#define DRV_QUAD_VER_MINOR                                                    09
#define DRV_QUAD_VER_PATH                                                      1

#define QUAD_BRANCH_MASTER

/* *****************************************************************************
 *        CONFIGURATION INSTRUCTIONS
***************************************************************************** */
/* Follow the steps below to properly set this driver up:                     */
/* - Set in the Setup file:                                                   */
/*    - TPM to be used: Default is TPM0. Quad shall have its exclusivity.     */
/*    - Sampling Frequency: Default is 1kHz. The faster the frequency, more   */
/*        precise the driver is, at the cost of higher CPU usage.             */
/*    - ID limit: Default is 4. The higher the value, the longer are the      */
/*        the interrupts and, therefore, it'll use more CPU.                  */
/* - In the Vectors.c file:                                                   */
/*    - Include this header file.                                             */
/*    - At the interrupt table add the interrupt prototype call to the proper */
/*        position. For example, if the TPM1 will be used, then at the TPM1   */
/*        set it to call the 'QUAD_TPM1_ISR' routine.                         */

/* *****************************************************************************
 *        DEFAULT CONFIGS
***************************************************************************** */
/* Use the labels below to select which TPM peripheral the driver will use    */
/*  as the base peripheral timer.                                             */
#define QUAD_USE_TIM1                                                          0
#define QUAD_USE_TIM2                                                          1
#define QUAD_USE_TIM3                                                          2
#define QUAD_USE_TIM4                                                          3
#define QUAD_USE_TIM8					                                         	       4
#define QUAD_USE_SYSTICK                                                       5

#ifndef QUAD_BASE_TMR
  #define QUAD_BASE_TMR                                            QUAD_USE_TIM1
#endif

#ifndef QUAD_MAX_AVAILABE_ID
  #define QUAD_MAX_AVAILABE_ID                                     	          10
#endif

#ifndef QUAD_DISABLE_FREQ_CALCULATION
  #define QUAD_DISABLE_FREQ_CALCULATION                                    FALSE
#endif

/* *****************************************************************************
 *        PUBLIC DEFINITIONS
***************************************************************************** */

typedef enum
{
  TIM1_ENCODER_CH1_CH2_AT_PE9_PE11 	 = 0,
  TIM2_ENCODER_CH1_CH2_AT_PA5_PB3 		,
  TIM3_ENCODER_CH1_CH2_AT_PA6_PA7		,
  TIM3_ENCODER_CH1_CH2_AT_PB4_PB5,
  TIM4_ENCODER_CH1_CH2_AT_PD12_PD13 	,
  TIM5_ENCODER_CH1_CH2_AT_PA0_PA1		,
  TIM8_ENCODER_CH1_CH2_AT_PC6_PC7		,
  QUAD_NUMBER_OF_PORTS,  /* For Sizing Only */
} QUAD_ENCODER_List;

typedef enum
{
	PULL_UP = 0,
	PULL_DOWN,
}QUAD_GPIO_Config_List;


typedef struct
{
	QUAD_ENCODER_List          QUAD_Routed;
	QUAD_GPIO_Config_List      QUAD_GPIO_Mode;
} QUAD_Parameters_t;

/* The definition below sets if the driver will use a FTM or the systick.     */
#if (QUAD_BASE_TMR >= QUAD_USE_TIM1) && (QUAD_BASE_TMR <= QUAD_USE_TIM8)
  #define QUAD_USING_FTM                                                    TRUE
#else
  #define QUAD_USING_FTM                                                   FALSE
#endif

/* *****************************************************************************
 *         PROTOTYPES
***************************************************************************** */

  ReturnCode_t QUAD_Init(uint8 ID, QUAD_Parameters_t * Encoder );
  ReturnCode_t QUAD_ReadPulses(uint8 ID, uint32 * Pulses );
  ReturnCode_t QUAD_ReadFrequency(uint8 ID, float * Freq );
  ReturnCode_t QUAD_Direction(uint8 ID, int8_t * Direction);
#endif
