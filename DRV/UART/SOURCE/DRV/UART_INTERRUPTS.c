/**
 ******************************************************************************
 * @file    UART_INTERRUPTS.h
 * @author  Plinio Barbosa da Silva
 * @brief   UART module driver.
 * @brief   Version 2019.11.1
 * @brief   Creation Date 28/03/2018
 *
 *          This file provides functions to manage UART
 *          functionalities:
 *           + Initialization of a UART
 *           + Read data from UART
 *           + Write data to UART
 */
#include "UART.h"
#include "stm32f4xx.h"
#include "sys_cfg_stm32f407.h"

/**
 * @brief  Initial Version V1.1.
 * @autor  Plinio Barbosa da Silva
 * @date   28 March 2018
 */
void USART1_IRQHandler(void)
{
  if((USART1->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE)
  {
    if((USART1->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
      CLEAR_BIT(USART1->SR, USART_SR_RXNE);
      UART1_Control.Last_Receive_Timestamp = Sys_Get_Tick();
      *UART1_Control.Receive_Buffer = USART1->DR;
      UART1_Control.Receive_Buffer ++;
      UART1_Control.Receive_Counter++;
    }
  }
  if((USART1->CR1 & USART_CR1_TCIE) == USART_CR1_TCIE)
  {
    if((USART1->SR & USART_SR_TC) == USART_SR_TC)
    {
      USART1->DR = *UART1_Control.Transmit_Buffer;
      UART1_Control.Transmit_Buffer ++;
      UART1_Control.Transmit_Counter++;
    }
  }
}

void USART2_IRQHandler(void)
{
  if((USART2->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE)
  {
    if((USART2->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
      CLEAR_BIT(USART2->SR, USART_SR_RXNE);
      UART2_Control.Last_Receive_Timestamp = Sys_Get_Tick();
      *UART2_Control.Receive_Buffer = USART2->DR;
      UART2_Control.Receive_Buffer ++;
      UART2_Control.Receive_Counter++;
    }
  }
  if((USART2->CR1 & USART_CR1_TCIE) == USART_CR1_TCIE)
  {
    if((USART2->SR & USART_SR_TC) == USART_SR_TC)
    {
      USART2->DR = *UART2_Control.Transmit_Buffer;
      UART2_Control.Transmit_Buffer ++;
      UART2_Control.Transmit_Counter++;
    }
  }
}

void USART3_IRQHandler(void)
{
  if((USART3->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE)
  {
    if((USART3->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
      CLEAR_BIT(USART3->SR, USART_SR_RXNE);
      UART3_Control.Last_Receive_Timestamp = Sys_Get_Tick();
      *UART3_Control.Receive_Buffer = USART3->DR;
      UART3_Control.Receive_Buffer ++;
      UART3_Control.Receive_Counter++;
    }
  }
  if((USART3->CR1 & USART_CR1_TCIE) == USART_CR1_TCIE)
  {
    if((USART3->SR & USART_SR_TC) == USART_SR_TC)
    {
      USART3->DR = *UART3_Control.Transmit_Buffer;
      UART3_Control.Transmit_Buffer ++;
      UART3_Control.Transmit_Counter++;
    }
  }
}

void USART6_IRQHandler(void)
{
  if((USART6->CR1 & USART_CR1_RXNEIE) == USART_CR1_RXNEIE)
  {
    if((USART6->SR & USART_SR_RXNE) == USART_SR_RXNE)
    {
      CLEAR_BIT(USART6->SR, USART_SR_RXNE);
      UART6_Control.Last_Receive_Timestamp = Sys_Get_Tick();
      *UART6_Control.Receive_Buffer = USART6->DR;
      UART6_Control.Receive_Buffer ++;
      UART6_Control.Receive_Counter++;
    }
  }
  if((USART6->CR1 & USART_CR1_TCIE) == USART_CR1_TCIE)
  {
    if((USART6->SR & USART_SR_TC) == USART_SR_TC)
    {
      USART6->DR = *UART6_Control.Transmit_Buffer;
      UART6_Control.Transmit_Buffer ++;
      UART6_Control.Transmit_Counter++;
    }
  }
}
