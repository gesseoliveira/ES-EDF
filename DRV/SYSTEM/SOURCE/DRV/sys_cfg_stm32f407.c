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


#include "sys_cfg_stm32f407.h"
#include "stm32f4xx.h"

#define NVIC_PRIORITYGROUP_4           0x00000003U
#define RCC_GET_FLAG(__FLAG__) (((((((__FLAG__) >> 5U) == 1U)? RCC->CR :((((__FLAG__) >> 5U) == 2U) ? RCC->BDCR :((((__FLAG__) >> 5U) == 3U)? RCC->CSR :RCC->CIR))) & (1U << ((__FLAG__) & 0x1FU)))!= 0U)? 1U : 0U)

uint32_t SysTick_Counter;

/**
 * @brief Enable the SYSCFG, COMP, VREFBUF clock and Power interface clock
 * @param  None
 * @retval None
 */
void Sys_Enable_Peripherals_Clock(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

  // Configure the main internal regulator output voltage
  MODIFY_REG(PWR->CR, PWR_CR_VOS, PWR_CR_VOS);
  // System configuration controller clock enable
  SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
  // Power interface clock enable
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
}

/**
 * @brief Configure the system clock to 168MHz
 * @param  None
 * @retval None
 */
void Sys_Configure_Clock_168MHz(void)
{

  /* Flash settings (see RM0090 rev9, p80) */
  FLASH->ACR = FLASH_ACR_LATENCY_5WS               /* 6 CPU cycle wait */
      | FLASH_ACR_PRFTEN                    /* enable prefetch */
      | FLASH_ACR_ICEN                      /* instruction cache enable */
      | FLASH_ACR_DCEN;                     /* data cache enable */

  /* Configure clocks
   * Max SYSCLK: 168MHz
   * Max AHB:  SYSCLK
   * Max APB1: SYSCLK/4 = 48MHz
   * Max APB2: SYSCLK/2 = 86MHz
   * + enable sys clock output 2 with clock divider = 4 */
  RCC->CFGR =   0                           /* Clock output 2 is SYSCLK (RCC_CFGR_MCO2) */
      | ( 0x6 << 27)                  /* Clock output divider */
      | RCC_CFGR_PPRE2_DIV2           /* APB2 prescaler */
      | RCC_CFGR_PPRE1_DIV4;          /* APB2 prescaler */

  /* Clock control register */
  RCC->CR = RCC_CR_HSEON;         /* Enable external oscillator */

  /* Wait for locked external oscillator */
  while((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY);

  /* PLL config */
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE                /* PLL source */
      | (4 << 0)                              /* PLL input division */
      | (168 << 6)                            /* PLL multiplication */
      | (0 << 16)                             /* PLL sys clock division */
      | (7 << 24);                            /* PLL usb clock division =48MHz */

  /* crystal:  8MHz
   * PLL in:   2MHz (div 4)
   * PLL loop: 336MHz (mul 168)
   * PLL out:  168MHz (div 2)
   * PLL usb:  48MHz (div 7)
   */

  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;


  /* Wait for locked PLL */
  while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

  /* select system clock */
  RCC->CFGR &= ~RCC_CFGR_SW; /* clear */
  RCC->CFGR |= RCC_CFGR_SW_PLL;   /* SYSCLK is PLL */

  /* Wait for SYSCLK to be PPL */
  while((RCC->CFGR & RCC_CFGR_SW_PLL) != RCC_CFGR_SW_PLL);


//for(uint32_t k = 0 ; k < 60000 ; k++)
//{}
  SystemCoreClock = 168000000; //System Clock set to 168MHz

  SysTick_Config(168000000 / 1000);
//  SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);   /* Configures the SysTick clock source to HCLK*/
}

/**
 * @brief Configure the system clock to 168MHz by HSI
 * @param  None
 * @retval None
 */
void Sys_Configure_Clock_168MHz_HSI(void)
{

//  /* Flash settings (see RM0090 rev9, p80) */
//  FLASH->ACR = FLASH_ACR_LATENCY_5WS               /* 6 CPU cycle wait */
//      | FLASH_ACR_PRFTEN                    /* enable prefetch */
//      | FLASH_ACR_ICEN                      /* instruction cache enable */
//      | FLASH_ACR_DCEN;                     /* data cache enable */

  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_5WS);

  MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 16 << RCC_CR_HSITRIM_Pos);

  /* Clock control register */
  SET_BIT(RCC->CR, RCC_CR_HSION);

  /* Wait for locked external oscillator */
  while((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY);

  /* Configure PLL used for SYSCLK Domain */
  MODIFY_REG(RCC->PLLCFGR,
      RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN,
      RCC_PLLCFGR_PLLSRC_HSI | RCC_PLLCFGR_PLLM_3 | 168 << RCC_PLLCFGR_PLLN_Pos);
  MODIFY_REG(RCC->PLLCFGR, RCC_PLLCFGR_PLLP, 0);

  /* Enable PLL */
  SET_BIT(RCC->CR, RCC_CR_PLLON);

  /* Wait for locked PLL */
  while((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY);

  /* Set prescaler */
  MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, 0);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV4);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV2);
  MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);

  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till System clock is ready */
  while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

  /* System Clock set to 168MHz */
  SystemCoreClock = 168000000;

  SysTick_Config(SystemCoreClock / 1000);

  /* Configures the SysTick clock source to HCLK*/
  SET_BIT(SysTick->CTRL, SysTick_CTRL_CLKSOURCE_Msk);
}

/**
 * @brief Set the system clock to reset default values
 * @param  None
 * @retval None
 */
void Sys_DeInit_Clock(void)
{

}

/**
 * @brief Enable the External Low Speed oscillator 32.678KHz
 * @param  None
 * @retval None
 */
void Sys_Enable_LSE(void)
{
  /* Enable write access to Backup domain */
  PWR->CR |= PWR_CR_DBP;
  /* Wait till Write access is enabled */
  while((PWR->CR & PWR_CR_DBP) == RESET);

  /* Enable the External Low Speed oscillator (LSE). */
  SET_BIT(RCC->BDCR, RCC_BDCR_LSEON);
  /* Wait till LSE is ready */
  while(RCC_GET_FLAG(0x41) == RESET);

  MODIFY_REG(RCC->CFGR, RCC_CFGR_RTCPRE, ((0x100U) & 0xFFFFCFFU));

  RCC->BDCR |= ((0x100U) & 0x00000FFFU);
  RCC->BDCR |= RCC_BDCR_RTCEN;
}

/**
 * @brief Return the system tick counter (miliseconds counter)
 * @param  None
 * @retval System milisecond tick value
 */

uint32_t Sys_Get_Tick(void)
{
  return (SysTick_Counter);
}



