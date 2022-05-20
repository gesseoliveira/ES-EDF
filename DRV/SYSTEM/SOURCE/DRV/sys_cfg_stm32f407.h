/**
 ******************************************************************************
 * @file    PIT.h
 * @author  Pl�nio Barbosa da Silva
 * @brief   MIcrocontroller System Clock and System Tick configuration.
 * @brief   Version 2019.8.2
 * @brief   Creation Date 12/07/2019
 *
 *          This file provides functions to manage RCC System and SysTickso
 *          functionalities:
 *           + RCC Initialization
 *           + Systick Initialization
 *           + Systick Interrupt Vector
 *
 */

#ifndef SYS_CFG_STM32F407_H_
#define SYS_CFG_STM32F407_H_

#include "stm32f4xx.h"

#define SYSTICK_CLKSOURCE_HCLK         0x00000004U
extern uint32_t SysTick_Counter;

/**
 * @brief Enable the SYSCFG, COMP, VREFBUF clock and Power interface clock
 * @param  None
 * @retval None
 */
void Sys_Enable_Peripherals_Clock(void);

/**
 * @brief Configure the system clock to 168MHz
 * @param  None
 * @retval None
 */
void Sys_Configure_Clock_168MHz(void);

/**
 * @brief Configure the system clock to 168MHz by HSI
 * @param  None
 * @retval None
 */
void Sys_Configure_Clock_168MHz_HSI(void);

/**
 * @brief Set the system clock to reset default values
 * @param  None
 * @retval None
 */
void Sys_DeInit_Clock(void);

/**
 * @brief Enable the External Low Speed oscillator 32.678KHz
 * @param  None
 * @retval None
 */
void Sys_Enable_LSE(void);

/**
 * @brief Return the system tick counter (miliseconds counter)
 * @param  None
 * @retval System milisecond tick value
 */
uint32_t	Sys_Get_Tick(void);

#endif /* SYS_STM32F4XX_H_ */
