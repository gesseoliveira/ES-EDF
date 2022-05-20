/**
 *******************************************************************************
 * @file    ADC_INTERRUPTS.c
 * @author  Peterson Aguiar
 * @brief   GPIO module driver.
 * @brief   Version 2019.7.2
 * @brief   Creation Date 07/05/2019
 *******************************************************************************
 */

#include "ADC.h"
#include "returncode.h"
#include "stm32f4xx.h"

#define ADC_DISABLE_IT(__INSTANCE__, __INTERRUPT__) (((__INSTANCE__)->CR1) &= ~(__INTERRUPT__))
#define ADC_GET_FLAG(__INSTANCE__, __FLAG__) ((((__INSTANCE__)->SR) & (__FLAG__)) == (__FLAG__))
#define ADC_CLEAR_FLAG(__INSTANCE__, __FLAG__) (((__INSTANCE__)->SR) = ~(__FLAG__))

/**
 * @brief This function handles ADC global interrupt.
 */
void ADC_IRQHandler(void)
{
  uint32_t EOC_1, EOC_2, EOC_3;

  EOC_1 = ADC_GET_FLAG(ADC1, ADC_SR_EOC);
  EOC_2 = ADC_GET_FLAG(ADC2, ADC_SR_EOC);
  EOC_3 = ADC_GET_FLAG(ADC3, ADC_SR_EOC);

  if (EOC_1 == 1)
  {
    Temporary_Buffer[0][Sample_Counter[0]++] =  ADC1->DR;
    if(Sample_Counter[0] >= Number_Of_Samples_To_Read[0])
    {
      ADC_DISABLE_IT(ADC1, (ADC_CR1_EOCIE | ADC_CR1_OVRIE));
      ADC_CLEAR_FLAG(ADC1, (ADC_SR_STRT | ADC_SR_EOC | ADC_SR_OVR));
    }
  }
  if (EOC_2 == 1)
  {
    Temporary_Buffer[1][Sample_Counter[1]++] =  ADC2->DR;
    if(Sample_Counter[1] >= Number_Of_Samples_To_Read[1])
    {
      ADC_DISABLE_IT(ADC2, (ADC_CR1_EOCIE | ADC_CR1_OVRIE));
      ADC_CLEAR_FLAG(ADC2, (ADC_SR_STRT | ADC_SR_EOC | ADC_SR_OVR));
    }
  }
  if (EOC_3 == 1)
  {
    Temporary_Buffer[2][Sample_Counter[2]++] =  ADC3->DR;
    if(Sample_Counter[2] >= Number_Of_Samples_To_Read[2])
    {
      ADC_DISABLE_IT(ADC3, (ADC_CR1_EOCIE | ADC_CR1_OVRIE));
      ADC_CLEAR_FLAG(ADC3, (ADC_SR_STRT | ADC_SR_EOC | ADC_SR_OVR));
    }
  }
}
