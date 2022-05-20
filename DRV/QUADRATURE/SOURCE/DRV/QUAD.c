/**
 ******************************************************************************
 * @file    QUAD.c
 * @author  Icaro Nascimento Queiroz
 * @brief   Quadrature module driver.
 * @brief   Version 2019.9.2
 * @brief   Creation Date 15/08/2019
 *
 *          This file provides functions to manage Encoder
 *          functionalities:
 *           + Initialization of a Timer as Encoder
 *           + Read pulses from Encoder
 *           + Get Frequency
 *           + Get Direction of spinning
 */
#include "QUAD.h"
#include "returncode.h"
#include "string.h"
#include "stm32f4xx.h"
#include "stdlib.h"

/*****************************************************************************************************************/
/*                                             GPIO ALTERNATIVE FUNCTIONS                                        */
/*                                                                                                               */
/*            Lembre que: AFR[0] -> GPIO0 até GPIO7                                                              */
/*            GPIOD->AFR[0] |= (ALTERNATE_FUNCTION_NUMBER << (4 * GPIO_NUMBER))                                  */
/*            Lembre que: AFR[1] -> GPIO8 até GPIO15                                                             */
/*            GPIOD->AFR[0] |= (ALTERNATE_FUNCTION_NUMBER << (4 * (GPIO_NUMBER - 8)))                            */
/*                                                                                                               */
/*****************************************************************************************************************/
#define SET_GPIO_AFRL(GPIO, GPIO_NUMBER, ALT_FUNC)((GPIO->AFR[0]) |= ((ALT_FUNC) << (4 * (GPIO_NUMBER))))
#define CLR_GPIO_AFRL(GPIO, GPIO_NUMBER)((GPIO->AFR[0]) &= ~((0x0F) << (4 * (GPIO_NUMBER))))

#define SET_GPIO_AFRH(GPIO, GPIO_NUMBER, ALT_FUNC)((GPIO->AFR[1]) |= ((ALT_FUNC) << (4 * (GPIO_NUMBER - 8))))
#define CLR_GPIO_AFRH(GPIO, GPIO_NUMBER)((GPIO->AFR[1]) &= ~(0x0F << (4 * (GPIO_NUMBER - 8))))

/*****************************************************************************************************************/
/*                                             GPIO MODE MACROS                                                  */
/*****************************************************************************************************************/
#define GPIO_MODER_MODE_INPUT 0x00
#define GPIO_MODER_MODE_OUTPUT 0x01
#define GPIO_MODER_MODE_ALT_FUNC 0x02
#define GPIO_MODER_MODE_ANALOG 0x03

#define SET_GPIO_MODE(GPIO, GPIO_NUMBER, MODE)  ((GPIO->MODER) |= ((MODE) << (2 * (GPIO_NUMBER))))
#define CLR_GPIO_MODE(GPIO, GPIO_NUMBER)        ((GPIO->MODER) &= ~((0x03) << (2 * (GPIO_NUMBER))))
/*****************************************************************************************************************/
/*                                             GPIO PULL-UP/PULL-DOWN MACROS                                     */
/*****************************************************************************************************************/
#define GPIO_PUPDR_NO_PULL 0x00
#define GPIO_PUPDR_PULL_UP 0x01
#define GPIO_PUPDR_PULL_DOWN 0x02
#define GPIO_PUPDR_PULL_RESERVED 0x03

#define SET_GPIO_PULL(GPIO, GPIO_NUMBER, MODE)  ((GPIO->PUPDR) |= ((MODE) << (2 * (GPIO_NUMBER))))
#define CLR_GPIO_PULL(GPIO, GPIO_NUMBER)        ((GPIO->PUPDR) &= ~((0x03) << (2 * (GPIO_NUMBER))))
/*****************************************************************************************************************/
/*                                             GPIO SPEED MACROS                                                 */
/*****************************************************************************************************************/
#define GPIO_OSPEED_LOW_SPEED_2MHZ 0x00
#define GPIO_OSPEED_MEDIUM_SPEED_25MHZ 0x01
#define GPIO_OSPEED_FAST_SPEED_50MHZ 0x02
#define GPIO_OSPEED_HIGH_SPEED_100MHZ 0x03

#define SET_GPIO_SPEED(GPIO, GPIO_NUMBER, MODE)  ((GPIO->OSPEEDR) |= ((MODE) << (2 * (GPIO_NUMBER))))
#define CLR_GPIO_SPEED(GPIO, GPIO_NUMBER)        ((GPIO->OSPEEDR) &= ~((0x03) << (2 * (GPIO_NUMBER))))

#define GPIO_SET_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) |=  (1UL << (BIT_NUM)))
#define GPIO_CLR_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) &= ~(1UL << (BIT_NUM)))
#define GPIO_TGL_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) ^=  (1UL << (BIT_NUM)))
#define GPIO_GET_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) &   (1UL << (BIT_NUM)))

/*Enable clock ports*/
#define GPIOA_CLK_ENABLE()	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)

#define GPIOB_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN)

#define GPIOC_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN)

#define GPIOD_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN)

#define GPIOE_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN)

#define GPIOF_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN)

#define GPIOG_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN)

#define GPIOH_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN)

#define GPIOI_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN)

#ifndef QUAD_DRV_PERIOD
	#define QUAD_DRV_PERIOD 											   65535
#endif

#define QUAD_DRV_OVERFLOW_CHECK				          (QUAD_DRV_PERIOD/2-(2700))

#ifndef QUAD_DRV_TIME_TO_GET_PULSE_FREQ
#define QUAD_DRV_TIME_TO_GET_PULSE_FREQ                                   50
#endif

#ifndef	QUAD_DRV_MAX_SAMPLES
#define QUAD_DRV_MAX_SAMPLES											    10
#endif

#ifndef QUAD_DRV_TIME_TO_SAMPLE
#define QUAD_DRV_TIME_TO_SAMPLE											  10
#endif

#ifndef QUAD_DRV_OVERFLOW_LIMIT_CHECK
	#define	QUAD_DRV_OVERFLOW_LIMIT_CHECK								    10000
#endif

typedef enum
{
  Unitialized			=0,
  Initialized			  ,
  Status_Error          ,
}Quad_Status;

typedef struct
{
  TIM_TypeDef		        *TIM_Used;
  uint32_t 				Pulses;
  float				    *Frequency;
  Quad_Status				Status;
  int8_t			        Direction;
}QUAD_Handler_t;

QUAD_Handler_t QUAD_Handler[QUAD_MAX_AVAILABE_ID];

uint32_t Samples[QUAD_MAX_AVAILABE_ID][QUAD_DRV_MAX_SAMPLES];
int8_t SampleIndex[QUAD_MAX_AVAILABE_ID];
uint32_t LastSample;
int16_t ActualPointer[QUAD_MAX_AVAILABE_ID];
int8_t LastDirection[QUAD_MAX_AVAILABE_ID] = {0};

/**
 * @brief  Quadrature Encoder configuration routine.
 * @param  ID ID that should be allocated and configured.
 * @param  Encoder Routed Encoder to the desired Quadrature encoder parameters
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t QUAD_Init(uint8 ID, QUAD_Parameters_t * Encoder )
{
  static ReturnCode_t ReturnValue = ANSWERED_REQUEST;
  static TIM_TypeDef *TIM_BASE;

  if(ID >= QUAD_MAX_AVAILABE_ID)
  {
    ReturnValue = ERR_PARAM_ID;
    QUAD_Handler[ID].Status = Status_Error;
  }
  if(QUAD_Handler[ID].Status == Initialized)
  {
    ReturnValue = ERR_ENABLED;
  }
  else
  {
    switch(Encoder->QUAD_Routed)
    {
      case TIM1_ENCODER_CH1_CH2_AT_PE9_PE11:
        TIM_BASE = TIM1;
        SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM1EN);
        GPIOE_CLK_ENABLE();
        /* Setting TIM1 GPIO Channels
         * TIM1_CH1->PE9
         * TIM1_CH2->PE11
         */
        /* Setting as Alternate Mode */
        CLR_GPIO_MODE(GPIOE, 9);
        CLR_GPIO_MODE(GPIOE, 11);
        SET_GPIO_MODE(GPIOE, 9, GPIO_MODER_MODE_ALT_FUNC);
        SET_GPIO_MODE(GPIOE, 11, GPIO_MODER_MODE_ALT_FUNC);
        /* Setting the mode of Alternate Function */
        CLR_GPIO_AFRH(GPIOE, 9);
        CLR_GPIO_AFRH(GPIOE, 11);
        SET_GPIO_AFRH(GPIOE, 9, 1);
        SET_GPIO_AFRH(GPIOE, 11, 1);
        /* Setting the GPIO Pull config */
        CLR_GPIO_PULL(GPIOE, 9);
        CLR_GPIO_PULL(GPIOE, 11);
        switch(Encoder->QUAD_GPIO_Mode)
        {
          case PULL_UP:
            SET_GPIO_PULL(GPIOE, 9, GPIO_PUPDR_PULL_UP);
            SET_GPIO_PULL(GPIOE, 11, GPIO_PUPDR_PULL_UP);
            break;
          case PULL_DOWN:
            SET_GPIO_PULL(GPIOE, 9, GPIO_PUPDR_PULL_DOWN);
            SET_GPIO_PULL(GPIOE, 11, GPIO_PUPDR_PULL_DOWN);
            break;
        }
        /* Setting the GPIO Speed */
        CLR_GPIO_SPEED(GPIOE, 9);
        CLR_GPIO_SPEED(GPIOE, 11);
        SET_GPIO_SPEED(GPIOE, 9, GPIO_OSPEED_LOW_SPEED_2MHZ);
        SET_GPIO_SPEED(GPIOE, 11, GPIO_OSPEED_LOW_SPEED_2MHZ);
        QUAD_Handler[ID].TIM_Used = TIM1;
        break;

      case TIM2_ENCODER_CH1_CH2_AT_PA5_PB3:
        TIM_BASE = TIM2;
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
        GPIOA_CLK_ENABLE();
        GPIOB_CLK_ENABLE();
        /* Setting TIM1 GPIO Channels
         * TIM2_CH1->PA5
         * TIM2_CH2->PB3
         */
        /* Setting as Alternate Mode */
        CLR_GPIO_MODE(GPIOA, 5);
        CLR_GPIO_MODE(GPIOB, 3);
        SET_GPIO_MODE(GPIOA, 5, GPIO_MODER_MODE_ALT_FUNC);
        SET_GPIO_MODE(GPIOB, 3, GPIO_MODER_MODE_ALT_FUNC);
        /* Setting the mode of Alternate Function */
        CLR_GPIO_AFRL(GPIOA, 5);
        CLR_GPIO_AFRL(GPIOB, 3);
        SET_GPIO_AFRL(GPIOA, 5, 1);
        SET_GPIO_AFRL(GPIOB, 3, 1);
        /* Setting the GPIO Pull config */
        CLR_GPIO_PULL(GPIOA, 5);
        CLR_GPIO_PULL(GPIOB, 3);
        switch(Encoder->QUAD_GPIO_Mode)
        {
          case PULL_UP:
            SET_GPIO_PULL(GPIOA, 5, GPIO_PUPDR_PULL_UP);
            SET_GPIO_PULL(GPIOB, 3, GPIO_PUPDR_PULL_UP);
            break;
          case PULL_DOWN:
            SET_GPIO_PULL(GPIOA, 5, GPIO_PUPDR_PULL_DOWN);
            SET_GPIO_PULL(GPIOB, 3, GPIO_PUPDR_PULL_DOWN);
            break;
        }
        /* Setting the GPIO Speed */
        CLR_GPIO_SPEED(GPIOA, 5);
        CLR_GPIO_SPEED(GPIOB, 3);
        SET_GPIO_SPEED(GPIOA, 5, GPIO_OSPEED_LOW_SPEED_2MHZ);
        SET_GPIO_SPEED(GPIOB, 3, GPIO_OSPEED_LOW_SPEED_2MHZ);
        QUAD_Handler[ID].TIM_Used = TIM2;
        break;

      case TIM3_ENCODER_CH1_CH2_AT_PA6_PA7:
        TIM_BASE = TIM3;
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
        GPIOA_CLK_ENABLE();
        /* Setting TIM1 GPIO Channels
         * TIM3_CH1->PA6
         * TIM3_CH2->PA7
         */
        /* Setting as Alternate Mode */
        CLR_GPIO_MODE(GPIOA, 6);
        CLR_GPIO_MODE(GPIOA, 7);
        SET_GPIO_MODE(GPIOA, 6, GPIO_MODER_MODE_ALT_FUNC);
        SET_GPIO_MODE(GPIOA, 7, GPIO_MODER_MODE_ALT_FUNC);
        /* Setting the mode of Alternate Function */
        CLR_GPIO_AFRL(GPIOA, 6);
        CLR_GPIO_AFRL(GPIOA, 7);
        SET_GPIO_AFRL(GPIOA, 6, 2);
        SET_GPIO_AFRL(GPIOA, 7, 2);
        /* Setting the GPIO Pull config */
        CLR_GPIO_PULL(GPIOA, 6);
        CLR_GPIO_PULL(GPIOA, 7);
        switch(Encoder->QUAD_GPIO_Mode)
        {
          case PULL_UP:
            SET_GPIO_PULL(GPIOA, 6, GPIO_PUPDR_PULL_UP);
            SET_GPIO_PULL(GPIOA, 7, GPIO_PUPDR_PULL_UP);
            break;
          case PULL_DOWN:
            SET_GPIO_PULL(GPIOA, 6, GPIO_PUPDR_PULL_DOWN);
            SET_GPIO_PULL(GPIOA, 7, GPIO_PUPDR_PULL_DOWN);
            break;
        }
        /* Setting the GPIO Speed */
        CLR_GPIO_SPEED(GPIOA, 6);
        CLR_GPIO_SPEED(GPIOA, 7);
        SET_GPIO_SPEED(GPIOA, 6, GPIO_OSPEED_LOW_SPEED_2MHZ);
        SET_GPIO_SPEED(GPIOA, 7, GPIO_OSPEED_LOW_SPEED_2MHZ);
        QUAD_Handler[ID].TIM_Used = TIM3;
        break;

      case TIM4_ENCODER_CH1_CH2_AT_PD12_PD13:
        TIM_BASE = TIM4;
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN);
        GPIOD_CLK_ENABLE();
        /* Setting TIM1 GPIO Channels
         * TIM4_CH1->PD12
         * TIM4_CH2->PD13
         */
        /* Setting as Alternate Mode */
        CLR_GPIO_MODE(GPIOD, 12);
        CLR_GPIO_MODE(GPIOD, 13);
        SET_GPIO_MODE(GPIOD, 12, GPIO_MODER_MODE_ALT_FUNC);
        SET_GPIO_MODE(GPIOD, 13, GPIO_MODER_MODE_ALT_FUNC);
        /* Setting the mode of Alternate Function */
        CLR_GPIO_AFRH(GPIOD, 12);
        CLR_GPIO_AFRH(GPIOD, 13);
        SET_GPIO_AFRH(GPIOD, 12, 2);
        SET_GPIO_AFRH(GPIOD, 13, 2);
        /* Setting the GPIO Pull config */
        CLR_GPIO_PULL(GPIOD, 12);
        CLR_GPIO_PULL(GPIOD, 13);
        switch(Encoder->QUAD_GPIO_Mode)
        {
          case PULL_UP:
            SET_GPIO_PULL(GPIOD, 12, GPIO_PUPDR_PULL_UP);
            SET_GPIO_PULL(GPIOD, 13, GPIO_PUPDR_PULL_UP);
            break;
          case PULL_DOWN:
            SET_GPIO_PULL(GPIOD, 12, GPIO_PUPDR_PULL_DOWN);
            SET_GPIO_PULL(GPIOD, 13, GPIO_PUPDR_PULL_DOWN);
            break;
        }
        /* Setting the GPIO Speed */
        CLR_GPIO_SPEED(GPIOD, 12);
        CLR_GPIO_SPEED(GPIOD, 13);
        SET_GPIO_SPEED(GPIOD, 12, GPIO_OSPEED_LOW_SPEED_2MHZ);
        SET_GPIO_SPEED(GPIOD, 13, GPIO_OSPEED_LOW_SPEED_2MHZ);
        QUAD_Handler[ID].TIM_Used = TIM4;
        break;

      case TIM5_ENCODER_CH1_CH2_AT_PA0_PA1:
        TIM_BASE = TIM5;
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM5EN);
        GPIOA_CLK_ENABLE();
        /* Setting TIM1 GPIO Channels
         * TIM5_CH1->PA0
         * TIM5_CH2->PA1
         */
        /* Setting as Alternate Mode */
        CLR_GPIO_MODE(GPIOA, 0);
        CLR_GPIO_MODE(GPIOA, 1);
        SET_GPIO_MODE(GPIOA, 0, GPIO_MODER_MODE_ALT_FUNC);
        SET_GPIO_MODE(GPIOA, 1, GPIO_MODER_MODE_ALT_FUNC);
        /* Setting the mode of Alternate Function */
        CLR_GPIO_AFRL(GPIOA, 0);
        CLR_GPIO_AFRL(GPIOA, 1);
        SET_GPIO_AFRL(GPIOA, 0, 2);
        SET_GPIO_AFRL(GPIOA, 1, 2);
        /* Setting the GPIO Pull config */
        CLR_GPIO_PULL(GPIOA, 0);
        CLR_GPIO_PULL(GPIOA, 1);
        switch(Encoder->QUAD_GPIO_Mode)
        {
          case PULL_UP:
            SET_GPIO_PULL(GPIOA, 0, GPIO_PUPDR_PULL_UP);
            SET_GPIO_PULL(GPIOA, 1, GPIO_PUPDR_PULL_UP);
            break;
          case PULL_DOWN:
            SET_GPIO_PULL(GPIOA, 0, GPIO_PUPDR_PULL_DOWN);
            SET_GPIO_PULL(GPIOA, 1, GPIO_PUPDR_PULL_DOWN);
            break;
        }
        /* Setting the GPIO Speed */
        CLR_GPIO_SPEED(GPIOA, 0);
        CLR_GPIO_SPEED(GPIOB, 1);
        SET_GPIO_SPEED(GPIOA, 0, GPIO_OSPEED_LOW_SPEED_2MHZ);
        SET_GPIO_SPEED(GPIOB, 1, GPIO_OSPEED_LOW_SPEED_2MHZ);
        QUAD_Handler[ID].TIM_Used = TIM5;
        break;

      case TIM8_ENCODER_CH1_CH2_AT_PC6_PC7:
            TIM_BASE = TIM8;
            SET_BIT(RCC->APB2ENR, RCC_APB2ENR_TIM8EN);
            GPIOC_CLK_ENABLE();
            /* Setting TIM8 GPIO Channels
             * TIM8_CH1->PC6
             * TIM8_CH2->PC7
             */
            /* Setting as Alternate Mode */
            CLR_GPIO_MODE(GPIOC, 6);
            CLR_GPIO_MODE(GPIOC, 7);
            SET_GPIO_MODE(GPIOC, 6, GPIO_MODER_MODE_ALT_FUNC);
            SET_GPIO_MODE(GPIOC, 7, GPIO_MODER_MODE_ALT_FUNC);
            /* Setting the mode of Alternate Function */
            CLR_GPIO_AFRL(GPIOC, 6);
            CLR_GPIO_AFRL(GPIOC, 7);
            SET_GPIO_AFRL(GPIOC, 6, 3);
            SET_GPIO_AFRL(GPIOC, 7, 3);
            /* Setting the GPIO Pull config */
            CLR_GPIO_PULL(GPIOC, 6);
            CLR_GPIO_PULL(GPIOC, 7);
            switch(Encoder->QUAD_GPIO_Mode)
            {
              case PULL_UP:
                SET_GPIO_PULL(GPIOC, 6, GPIO_PUPDR_PULL_UP);
                SET_GPIO_PULL(GPIOC, 7, GPIO_PUPDR_PULL_UP);
                break;
              case PULL_DOWN:
                SET_GPIO_PULL(GPIOC, 6, GPIO_PUPDR_PULL_DOWN);
                SET_GPIO_PULL(GPIOC, 7, GPIO_PUPDR_PULL_DOWN);
                break;
            }
            /* Setting the GPIO Speed */
            CLR_GPIO_SPEED(GPIOC, 6);
            CLR_GPIO_SPEED(GPIOC, 7);
            SET_GPIO_SPEED(GPIOC, 6, GPIO_OSPEED_LOW_SPEED_2MHZ);
            SET_GPIO_SPEED(GPIOC, 7, GPIO_OSPEED_LOW_SPEED_2MHZ);
            QUAD_Handler[ID].TIM_Used = TIM8;
            break;

      case TIM3_ENCODER_CH1_CH2_AT_PB4_PB5:
        TIM_BASE = TIM3;
        SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
        GPIOB_CLK_ENABLE();
        /* Setting TIM1 GPIO Channels
         * TIM3_CH1->PB4
         * TIM3_CH2->PB5
         */
        /* Setting as Alternate Mode */
        CLR_GPIO_MODE(GPIOB, 4);
        CLR_GPIO_MODE(GPIOB, 5);
        SET_GPIO_MODE(GPIOB, 4, GPIO_MODER_MODE_ALT_FUNC);
        SET_GPIO_MODE(GPIOB, 5, GPIO_MODER_MODE_ALT_FUNC);
        /* Setting the mode of Alternate Function */
        CLR_GPIO_AFRL(GPIOB, 4);
        CLR_GPIO_AFRL(GPIOB, 5);
        SET_GPIO_AFRL(GPIOB, 4, 2);
        SET_GPIO_AFRL(GPIOB, 5, 2);
        /* Setting the GPIO Pull config */
        CLR_GPIO_PULL(GPIOB, 4);
        CLR_GPIO_PULL(GPIOB, 5);
        switch(Encoder->QUAD_GPIO_Mode)
        {
          case PULL_UP:
            SET_GPIO_PULL(GPIOB, 4, GPIO_PUPDR_PULL_UP);
            SET_GPIO_PULL(GPIOB, 5, GPIO_PUPDR_PULL_UP);
            break;
          case PULL_DOWN:
            SET_GPIO_PULL(GPIOB, 4, GPIO_PUPDR_PULL_DOWN);
            SET_GPIO_PULL(GPIOB, 5, GPIO_PUPDR_PULL_DOWN);
            break;
        }
        /* Setting the GPIO Speed */
        CLR_GPIO_SPEED(GPIOB, 4);
        CLR_GPIO_SPEED(GPIOB, 5);
        SET_GPIO_SPEED(GPIOB, 4, GPIO_OSPEED_LOW_SPEED_2MHZ);
        SET_GPIO_SPEED(GPIOB, 5, GPIO_OSPEED_LOW_SPEED_2MHZ);
        QUAD_Handler[ID].TIM_Used = TIM3;
        break;

      default:
        QUAD_Handler[ID].Status = Status_Error;
        ReturnValue = ERR_PARAM_VALUE;
        return ReturnValue;
        break;
    }

    /* Timer Counter Mode */
    TIM_BASE->CR1 &= ~(TIM_CR1_DIR | TIM_CR1_CMS);
    TIM_BASE->CR1 |= 0x00000000U;

    /* Clock Division */
    TIM_BASE->CR1 &= ~TIM_CR1_CKD;
    TIM_BASE->CR1 |= (uint32_t)0x00000000U;

    /* Preload */
    MODIFY_REG(TIM_BASE->CR1, TIM_CR1_ARPE, 0x00000000U);

    /* Period */
    TIM_BASE->ARR = (uint32_t)QUAD_DRV_PERIOD;

    /* Prescaler Divider */
    TIM_BASE->PSC = (uint32_t)0U;
    TIM_BASE->EGR = TIM_EGR_UG;
    TIM_BASE->SMCR &= ~(TIM_SMCR_SMS | TIM_SMCR_ECE);
    TIM_BASE->SMCR |= TIM_SMCR_SMS_0;
    TIM_BASE->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    TIM_BASE->CCMR1 |= (TIM_CCMR1_CC1S_0 | (TIM_CCMR1_CC1S_0 << 8U));
    TIM_BASE->CCMR1 &= ~(TIM_CCMR1_IC1PSC_Msk | TIM_CCMR1_IC2PSC_Msk);
    TIM_BASE->CCMR1 &= ~(TIM_CCMR1_IC2PSC | TIM_CCMR1_IC2F);

    /* Prescaler */
    TIM_BASE->CCMR1 |= 0x00000000U | (0x00000000U << 8U);

    /* Filter */
    TIM_BASE->CCMR1 |= (0x00000000U << 4U) | (0x00000000U << 12U);

    TIM_BASE->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC2P);
    TIM_BASE->CCER &= ~(TIM_CCER_CC1NP | TIM_CCER_CC2NP);
    /* Polarity */
    TIM_BASE->CCER |= 0x00000000U | (0x00000000U << 4U);

    /* Master Trigger */
    TIM_BASE->CR2 &= ~TIM_CR2_MMS;
    TIM_BASE->CR2 |= 0x00000000U;

    /* Master Config*/
    TIM_BASE->SMCR &= ~TIM_SMCR_MSM;
    TIM_BASE->SMCR |= 0x00000000U;

    /* TIM Channel */
    TIM_BASE->CCER &= ~(TIM_CCER_CC1E << (0x0000003CU & 0x1FU));
    TIM_BASE->CCER |= (uint32_t)(0x00000001U << (0x0000003CU & 0x1FU));
    TIM_BASE->CR1 |= TIM_CR1_CEN;

    QUAD_Handler[ID].Status = Initialized;
    ReturnValue = ANSWERED_REQUEST;

    //TIM_BASE->CNT = 1000;
  }
  return ReturnValue;
}

/**
 * @brief  Quadrature Encoder pulse counter routine.
 * @param  ID ID that should be allocated and configured.
 * @param  Pulses Variable that receives the count of encoder pulses
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t QUAD_ReadPulses(uint8 ID, uint32_t * Pulses )
{
  static ReturnCode_t ReturnValue = ANSWERED_REQUEST;
  static uint32_t     Actual_Time[QUAD_MAX_AVAILABE_ID];
  static uint32_t     Initial_Time[QUAD_MAX_AVAILABE_ID];

  if(QUAD_Handler[ID].Status == Initialized)
  {
    /*Get Current time                                                        */
    Actual_Time[ID] = Sys_Get_Tick();

    /*Store current pulse read                                                */
    /*Note:Below is dived by 2, but this action is unknown                    */
    QUAD_Handler[ID].Pulses = (QUAD_Handler[ID].TIM_Used->CNT)/2;
    *Pulses = QUAD_Handler[ID].Pulses;

    /*Check if Time for sample have expired                                   */
    if( (Actual_Time[ID] - Initial_Time[ID]) >=  QUAD_DRV_TIME_TO_SAMPLE)
    {
      /*Get current time                                                      */
      Initial_Time[ID] = Sys_Get_Tick();

      /*If limit samples occurred the, restart vector sample                  */
      if(SampleIndex[ID] >= QUAD_DRV_MAX_SAMPLES )
      {
        /*Restart vector sample                                               */
        SampleIndex[ID] = 0;
        Samples[ID][0]  = Samples[ID][QUAD_DRV_MAX_SAMPLES-1];
      }

      /*************************************************************************
       * Below have been implemented the simple filtering approach to get
       * correctness value upon read pulse sample.
       ************************************************************************/
      /*Eliminate reading noise by checking if is bigger/lower than 1 pulse
       * unit                                                               */
      if(Samples[ID][SampleIndex[ID] - 1] !=  *Pulses )
      {
        Samples[ID][SampleIndex[ID]] = *Pulses;

        /* Verifies if the direction is already defined */
        if(LastDirection[ID] == 0)
        {
          /*New direction analyze, done by derivation upon current sample and
           * last sample                                                      */
          if ( (QUAD_Handler[ID].TIM_Used->CNT) > QUAD_DRV_OVERFLOW_CHECK )
          {
            /* Start spinning on negative direction */
            QUAD_Handler[ID].Direction = -1;
            LastDirection[ID] = -1;
          }
          else
          {
            /* Start spinning on positive direction */
            QUAD_Handler[ID].Direction = 1;
            LastDirection[ID] = 1;
          }
        }
        /* Direction is defined */
        else
        {
          /* Encoder on positive direction of spinning */
          if(LastDirection[ID] == 1)
          {
            /* Verifies the Samples vector to detect overflow on
             * measurement                                                */
            if( Samples[ID][SampleIndex[ID]] < Samples[ID][SampleIndex[ID]-1] )
            {
              /* Check the samples deviation to detect a overflow on
               * actual direction */
              if(((Samples[ID][SampleIndex[ID]-1] - Samples[ID][SampleIndex[ID]]) > QUAD_DRV_OVERFLOW_CHECK ))
              {
                /* Overflow on previous direction */
                QUAD_Handler[ID].Direction = 1;
                LastDirection[ID] = 1;
              }
              else
              {
                /* Direction change */
                QUAD_Handler[ID].Direction = -1;
                LastDirection[ID] = -1;
              }
            }
            /* No overflow detected */
            else
            {
              /* Verifies the value of sample index for the first value */
              if(SampleIndex[ID] > 0)
              {
                /* Detect if the deviation between the samples is
                 * lower than limits of overflow                      */
                if(Samples[ID][SampleIndex[ID]-1] < QUAD_DRV_OVERFLOW_LIMIT_CHECK && Samples[ID][SampleIndex[ID]] >= QUAD_DRV_OVERFLOW_CHECK)
                {
                  /* The encoder has change the direction of
                   * spinning                                       */
                  QUAD_Handler[ID].Direction = -1;
                  LastDirection[ID] = -1;
                }
                else
                {
                  /* Encoder still spinning on same direction */
                  QUAD_Handler[ID].Direction = 1;
                  LastDirection[ID] = 1;
                }
              }
              else
              {
                /* Verifies the actual pulse and compares with the
                 *  actual sample from Vector of samples              */
                if(QUAD_Handler[ID].Pulses >= QUAD_DRV_OVERFLOW_CHECK && Samples[ID][QUAD_DRV_MAX_SAMPLES - 1] < QUAD_DRV_OVERFLOW_LIMIT_CHECK )
                {
                  /* The encoder has change the direction of
                   * spinning                                       */
                  QUAD_Handler[ID].Direction = -1;
                  LastDirection[ID] = -1;
                }
                else
                {
                  /* Encoder still spinning on same direction */
                  QUAD_Handler[ID].Direction = 1;
                  LastDirection[ID] = 1;
                }
              }
            }
          }
          /* Encoder on negative direction of spinning */
          else
          {
            /* Verifies the Samples vector to detect overflow on
             * measurement                                                */
            if( Samples[ID][SampleIndex[ID]] > Samples[ID][SampleIndex[ID]-1] )
            {
              /* Get the direction and last value from measurement of
               * pulses                                                 */
              if(((Samples[ID][SampleIndex[ID]] - Samples[ID][SampleIndex[ID]-1]) > QUAD_DRV_OVERFLOW_CHECK ))
              {
                /* Overflow on previous direction */
                QUAD_Handler[ID].Direction = -1;
                LastDirection[ID] = -1;
              }
              else
              {
                /* Direction change */
                QUAD_Handler[ID].Direction = 1;
                LastDirection[ID] = 1;
              }
            }
            /* No overflow detected */
            else
            {
              /* Verifies the value of sample index for the first value */
              if(SampleIndex[ID] > 0)
              {
                /* Detect if the deviation between the samples is
                 * lower than limits of overflow                      */
                if(Samples[ID][SampleIndex[ID]-1] >= QUAD_DRV_OVERFLOW_CHECK && Samples[ID][SampleIndex[ID]] < QUAD_DRV_OVERFLOW_LIMIT_CHECK )
                {
                  /* The encoder has change the direction of
                   * spinning                                       */
                  QUAD_Handler[ID].Direction = 1;
                  LastDirection[ID] = 1;
                }
                else
                {
                  /* Encoder still spinning on same direction */
                  QUAD_Handler[ID].Direction = -1;
                  LastDirection[ID] = -1;
                }
              }
              else
              {
                /* Verifies the actual pulse and compares with the
                 *  actual sample from Vector of samples              */
                if(QUAD_Handler[ID].Pulses >= QUAD_DRV_OVERFLOW_CHECK && Samples[ID][QUAD_DRV_MAX_SAMPLES - 1] < QUAD_DRV_OVERFLOW_LIMIT_CHECK )
                {
                  /* The encoder has change the direction of
                   * spinning                                       */
                  QUAD_Handler[ID].Direction = 1;
                  LastDirection[ID] = 1;
                }
                else
                {
                  /* Encoder still spinning on same direction */
                  QUAD_Handler[ID].Direction = 1;
                  LastDirection[ID] = 1;
                }
              }
            }
          }
        }
        /*Increment index sample                                            */
        SampleIndex[ID]++;
      }
    }
  }
  else
  {
    ReturnValue = ERR_DISABLED;
  }
  return ReturnValue;
}

/**
 * @brief  Quadrature read frequency routine.
 * @param  ID ID that should be allocated and configured.
 * @param  Freq Frequency read from samples of Encoder counter
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t QUAD_ReadFrequency(uint8 ID, float * Freq )
{
  typedef enum
  {
    START 		   =0,
    READ			 ,
  }Read_Freq_State;

  static ReturnCode_t  			ReturnValue[QUAD_MAX_AVAILABE_ID];
  static Read_Freq_State 			State[QUAD_MAX_AVAILABE_ID];
  static uint32_t 				    Initial_Time[QUAD_MAX_AVAILABE_ID],
								              Actual_Time[QUAD_MAX_AVAILABE_ID],
								              Initial_Pulse[QUAD_MAX_AVAILABE_ID],
								              Final_Pulse[QUAD_MAX_AVAILABE_ID];
  static int8_t					Direction_Pulse;
  if(QUAD_Handler[ID].Status == Initialized)
  {
    switch(State[ID])
    {
      case START:
        /* Get the initial time of execution */
        Initial_Time[ID] = Sys_Get_Tick();
        /* Get the actual time of execution */
        Actual_Time[ID] = Sys_Get_Tick();
        /* Get initial value of pulse read */
        Initial_Pulse[ID] = (QUAD_Handler[ID].TIM_Used->CNT)/2;
        /* Go for next state of operation */
        State[ID] = READ;
        ReturnValue[ID] = OPERATION_RUNNING;
        break;

      case READ:
        /* Get actual time of operation */
        Actual_Time[ID] = Sys_Get_Tick();
        /* Verifies if the direction is already defined */
        if(LastDirection[ID] == 0)
        {
          /*New direction analyze, done by derivation upon current sample and
           * last sample                                                     */
          if (  Samples[ID][SampleIndex[ID]] < Samples[ID][SampleIndex[ID]-1]  )
          {
            QUAD_Handler[ID].Direction = -1;
            LastDirection[ID] = -1;
          }
          else
          {
            QUAD_Handler[ID].Direction = 1;
            LastDirection[ID] = 1;
          }
        }
        /* Get the final value of pulse read */
        Final_Pulse[ID] = (QUAD_Handler[ID].TIM_Used->CNT)/2;
        /* Verifies if the time for sample the frequency is reached*/
        if((Actual_Time[ID] - Initial_Time[ID]) >= QUAD_DRV_TIME_TO_GET_PULSE_FREQ)
        {
          /* Get the direction of spinning */
          QUAD_Direction(ID, &Direction_Pulse);
          /* Encoder on positive spinning */
          if(Direction_Pulse == 1)
          {
          /* Verifies a overflow event */
            if(Final_Pulse[ID] < Initial_Pulse[ID])
            {
              if(abs((Initial_Pulse[ID] - Final_Pulse[ID])) < QUAD_DRV_OVERFLOW_LIMIT_CHECK)
              {
                *Freq = (float)((Initial_Pulse[ID] - Final_Pulse[ID]) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
              else
              {
                /* Get the frequency value with a overflow event */
                *Freq = (float)(((((QUAD_DRV_PERIOD/2) - Initial_Pulse[ID] + Final_Pulse[ID] ) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0 ))));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
            }
            /* No overflow detected */
            else
            {
              if(abs((Initial_Pulse[ID] - Final_Pulse[ID])) > QUAD_DRV_OVERFLOW_CHECK)
              {
                *Freq = (float)(((((QUAD_DRV_PERIOD/2) + Initial_Pulse[ID] - Final_Pulse[ID] ) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0 ))));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
              else
              {
                /* Get the frequency value with pulses value deviation */
                *Freq = (float)((((Final_Pulse[ID] - Initial_Pulse[ID]) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0 ))));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
            }
          }
          /* Encoder on negative spinning */
          else
          {
          /* Verifies a overflow event */
            if(Final_Pulse[ID] > Initial_Pulse[ID])
            {
              if(abs((Initial_Pulse[ID] - Final_Pulse[ID])) < QUAD_DRV_OVERFLOW_LIMIT_CHECK)
              {
                *Freq = (float)((Final_Pulse[ID] - Initial_Pulse[ID]) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
                else
                {
                  /* Get the frequency value with a overflow event */
                  *Freq = (float)(((((QUAD_DRV_PERIOD/2) - Final_Pulse[ID]) +  Initial_Pulse[ID]) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0 )));
                  QUAD_Handler[ID].Frequency = Freq;
                  ReturnValue[ID] = ANSWERED_REQUEST;
                  State[ID] = START;
                }
            }
            /* No overflow detected */
            else
            {
              if(abs((Initial_Pulse[ID] - Final_Pulse[ID])) > QUAD_DRV_OVERFLOW_CHECK)
              {
                *Freq = (float)(((((QUAD_DRV_PERIOD/2) - Initial_Pulse[ID] + Final_Pulse[ID] ) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0 ))));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
              else
              {
                /* Get the frequency value with pulses value deviation */
                *Freq = (float)((((Initial_Pulse[ID] - Final_Pulse[ID]) / ((float)(Actual_Time[ID] - Initial_Time[ID]) / 1000.0 ))));
                QUAD_Handler[ID].Frequency = Freq;
                ReturnValue[ID] = ANSWERED_REQUEST;
                State[ID] = START;
              }
            }
          }
        }
        break;
    }
  }
  else
  {
    ReturnValue[ID] = ERR_FAILED;
  }
  return ReturnValue[ID];
}

/**
 * @brief  Quadrature get direction of spin routine.
 * @param  ID ID that should be allocated and configured.
 * @param  Direction Direction of spin
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t QUAD_Direction(uint8 ID, int8_t * Direction)
{

  ReturnCode_t ReturnValue;
  ReturnValue = ANSWERED_REQUEST;

  if(LastDirection[ID] == 0)
  {
	  QUAD_Handler[ID].Direction = LastDirection[ID];
	  *Direction = QUAD_Handler[ID].Direction;
  }
  *Direction = QUAD_Handler[ID].Direction;

  return ReturnValue;
}
