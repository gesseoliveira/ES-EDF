/**
 *******************************************************************************
 * @file    ADC.c
 * @author  Peterson Aguiar
 * @brief   GPIO module driver.
 * @brief   Version 2019.8.1
 * @brief   Creation Date 07/05/2019
 *******************************************************************************
 */

#include "ADC.h"
#include "stm32f4xx.h"

/** @defgroup CLK Enable
 * @{
 */
#define GPIOA_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)
#define GPIOB_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN)
#define GPIOC_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN)
/**
 * @}
 */

/** @defgroup GPIO Mode Macros
 * @{
 */
#define GPIO_MODER_MODE_INPUT 0x00
#define GPIO_MODER_MODE_OUTPUT 0x01
#define GPIO_MODER_MODE_ALT_FUNC 0x02
#define GPIO_MODER_MODE_ANALOG 0x03

#define SET_GPIO_MODE(GPIO, GPIO_NUMBER, MODE)  ((GPIO->MODER) |= ((MODE) << (2 * (GPIO_NUMBER))))
#define CLR_GPIO_MODE(GPIO, GPIO_NUMBER)        ((GPIO->MODER) &= ~((0x03) << (2 * (GPIO_NUMBER))))
/**
 * @}
 */

/** @defgroup GPIO Pull-Up/Pull-Down Macros
 * @{
 */
#define GPIO_PUPDR_NO_PULL 0x00
#define GPIO_PUPDR_PULL_UP 0x01
#define GPIO_PUPDR_PULL_DOWN 0x02
#define GPIO_PUPDR_PULL_RESERVED 0x03
#define SET_GPIO_PULL(GPIO, GPIO_NUMBER, MODE)  ((GPIO->PUPDR) |= ((MODE) << (2 * (GPIO_NUMBER))))
#define CLR_GPIO_PULL(GPIO, GPIO_NUMBER)        ((GPIO->PUPDR) &= ~((0x03) << (2 * (GPIO_NUMBER))))
/**
 * @}
 */

/** @defgroup ADC Macros
 * @{
 */
#define ADC_SQR1(_NbrOfConversion_) (((_NbrOfConversion_) - (uint8_t)1U) << 20U)
#define ADC_SQR3_RK(_CHANNELNB_, _RANKNB_) (((uint32_t)((uint16_t)(_CHANNELNB_))) << (5U * ((_RANKNB_) - 1U)))
/**
 * @}
 */

uint32_t *Temporary_Buffer[ADC_MAX_ID];
uint32_t Number_Of_Samples_To_Read[ADC_MAX_ID] = {0};
uint32_t Sample_Counter[ADC_MAX_ID];

uint8_t Flag_ADC_Initialized[ADC_MAX_ID] = {0};

ADC_Channel_Config_t  ADC_Channel_List[ADC_MAX_CHANNEL_ID];
ADC_Parameters_t      ADC_Hardware_List[ADC_MAX_ID];

enum
{
  UNLOCKED,
  LOCKED,
  LOCK_OK,
  LOCK_ERROR
};
typedef struct{
  uint8_t     ID;     // ID who locked the mutex
  uint8_t     Lock;   // 0 = Unlocked ; 1 = Locked
}ADC_Driver_Mutex_t;

ADC_Driver_Mutex_t ADC_Channels_Mutex[ADC_MAX_ID];

uint8_t ADC_Driver_Mutex_Try_Lock(uint8_t ID, ADC_Driver_Mutex_t *Mutex);

static void ADC_Driver_Mutex_Release(ADC_Driver_Mutex_t *Mutex);

uint8_t ADC_Driver_Mutex_Try_Lock(uint8_t ID, ADC_Driver_Mutex_t *Mutex)
{
  uint8_t Mutex_Result;
  if(Mutex->Lock == UNLOCKED)
  {
    Mutex->Lock = LOCKED;
    Mutex->ID = ID;
    Mutex_Result = LOCK_OK; // Lock success.
  }
  else if((Mutex->Lock == LOCKED) && (Mutex->ID == ID))
  {
    Mutex_Result = LOCK_OK; // Already locked by the same ID. Return sucess.
  }
  else if((Mutex->Lock == LOCKED) && !(Mutex->ID == ID))
  {
    Mutex_Result = LOCK_ERROR; // Return lock error. The mutex is locked by another ID.
  }

  return Mutex_Result;
}

static void ADC_Driver_Mutex_Release(ADC_Driver_Mutex_t *Mutex)
{
  Mutex->Lock = UNLOCKED;
}

/**
 * @brief  Initialize the internal ADC.
 * @param  ID: ID of the ADC
 * @param  ADC_Configuration: ADC Configuration structure @ref ADC_Parameters_t
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 * @note   This routine will configure the ADC to perform reading on ADC channels
 *         using the Interrupt.
 *
 */
ReturnCode_t ADC_Init(uint8_t ID, ADC_Parameters_t ADC_Configuration)
{
  ReturnCode_t RetCode;
  ADC_TypeDef  *Instance;
  if(ID <= ADC_MAX_ID)
  {
    if(Flag_ADC_Initialized[ID] == 0)
    {
      /**Configure the global features of the ADC (Clock, Resolution, Data
       * Alignment and number of conversion)
       */
      switch (ADC_Configuration.ADC_Hardware)
      {
        case ADC_1:
          /* Enable Clock */
          SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);
          Instance = ADC1;
          break;
        case ADC_2:
          /* Enable Clock */
          SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC2EN);
          Instance = ADC2;
          break;
        case ADC_3:
          /* Enable Clock */
          SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC3EN);
          Instance = ADC3;
          break;
      }

      RetCode = ERR_DEVICE;

      /* Set the ADC clock prescaler */
      ADC123_COMMON->CCR &= ~(ADC_CCR_ADCPRE);
      ADC123_COMMON->CCR |=  ADC_Configuration.Clock_Prescaler;

      /* Set ADC scan mode */
      Instance->CR1 &= ~(ADC_CR1_SCAN);
      Instance->CR1 |= (DISABLE << 8U);

      /* Set ADC resolution */
      Instance->CR1 &= ~(ADC_CR1_RES);
      Instance->CR1 |=  ADC_Configuration.Output_Resolution;

      /* Clear ADC data alignment */
      Instance->CR2 &= ~(ADC_CR2_ALIGN);

      /* Reset the external trigger */
      Instance->CR2 &= ~(ADC_CR2_EXTSEL);
      Instance->CR2 &= ~(ADC_CR2_EXTEN);

      /* Enable or disable ADC continuous conversion mode */
      Instance->CR2 &= ~(ADC_CR2_CONT);
      Instance->CR2 |= (ENABLE << 1U);
      /* Disable the selected ADC regular discontinuous mode */
      Instance->CR1 &= ~(ADC_CR1_DISCEN);

      /* Set ADC number of conversion (Only one, it will be changed on channel read) */
      Instance->SQR1 &= ~(ADC_SQR1_L);
      Instance->SQR1 |= (0 << 20U);

      /* Enable or disable ADC DMA continuous request */
      Instance->CR2 &= ~(ADC_CR2_DDS);
      Instance->CR2 |= (DISABLE  << 9U);

      /* Enable or disable ADC end of conversion selection */
      Instance->CR2 &= ~(ADC_CR2_EOCS);
      Instance->CR2 |= (0 << 10U);

      ADC_Hardware_List[ID].ADC_Hardware = ADC_Configuration.ADC_Hardware;
      ADC_Hardware_List[ID].Clock_Prescaler = ADC_Configuration.Clock_Prescaler;
      ADC_Hardware_List[ID].Output_Resolution = ADC_Configuration.Output_Resolution;

      NVIC_SetPriority(ADC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
      NVIC_EnableIRQ(ADC_IRQn);

      Instance->CR2 |=  ADC_CR2_ADON;

      Flag_ADC_Initialized[ID] = 1; // Mark if the ADC is already initialized

      RetCode = ANSWERED_REQUEST;
    }
    else
    {
      RetCode = ERR_ENABLED;
    }
  }
  else
  {
    RetCode = ERR_PARAM_ID;
  }
  return RetCode;
}

/**
 * @brief  Initialize an specified channel of ADC
 * @param  ID: ID of the channel
 * @param  ID_ADC: ID of the initialized ADC
 * @param  ADC_Channel_Setup: ADC Configuration structure @ref ADC_Channel_Config_t
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 * @note   This routine will configure the ADC channel, and associates to a ID.
 *
 */
ReturnCode_t ADC_Setup_Channel(uint8_t ID_Channel, uint8_t ID_ADC, ADC_Channel_Config_t ADC_Channel_Setup)
{
  ReturnCode_t RetCode;

  RetCode = ANSWERED_REQUEST;
  if(ID_Channel <= ADC_MAX_CHANNEL_ID)
  {
    ADC_Channel_List[ID_Channel].Channel                = ADC_Channel_Setup.Channel;
    ADC_Channel_List[ID_Channel].Channel_Sampling_Time  = ADC_Channel_Setup.Channel_Sampling_Time;
    ADC_Channel_List[ID_Channel].ID_ADC                 = ID_ADC;

    switch(ADC_Channel_List[ID_Channel].Channel)
    {
      case ADC123_CH_0_AT_PA0:  // PA0
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 0);
        SET_GPIO_MODE(GPIOA, 0, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 0);
        SET_GPIO_PULL(GPIOA, 0, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_1_AT_PA1:  // PA1
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 1);
        SET_GPIO_MODE(GPIOA, 1, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 1);
        SET_GPIO_PULL(GPIOA, 1, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_2_AT_PA2:  // PA2
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 2);
        SET_GPIO_MODE(GPIOA, 2, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 2);
        SET_GPIO_PULL(GPIOA, 2, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_3_AT_PA3:  // PA3
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 3);
        SET_GPIO_MODE(GPIOA, 3, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 3);
        SET_GPIO_PULL(GPIOA, 3, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_4_AT_PA4:  // PA4
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 4);
        SET_GPIO_MODE(GPIOA, 4, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 4);
        SET_GPIO_PULL(GPIOA, 4, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_5_AT_PA5:  // PA5
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 5);
        SET_GPIO_MODE(GPIOA, 5, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 5);
        SET_GPIO_PULL(GPIOA, 5, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_6_AT_PA6:  // PA6
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 6);
        SET_GPIO_MODE(GPIOA, 6, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 6);
        SET_GPIO_PULL(GPIOA, 6, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_7_AT_PA7:  // PA7
        GPIOA_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOA, 7);
        SET_GPIO_MODE(GPIOA, 7, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOA, 7);
        SET_GPIO_PULL(GPIOA, 7, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_8_AT_PB0:  // PB0
        GPIOB_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOB, 0);
        SET_GPIO_MODE(GPIOB, 0, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOB, 0);
        SET_GPIO_PULL(GPIOB, 0, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_9_AT_PB1:  // PB1
        GPIOB_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOB, 1);
        SET_GPIO_MODE(GPIOB, 1, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOB, 1);
        SET_GPIO_PULL(GPIOB, 1, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_10_AT_PC0:  // PC0
        GPIOC_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOC, 0);
        SET_GPIO_MODE(GPIOC, 0, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOC, 0);
        SET_GPIO_PULL(GPIOC, 0, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_11_AT_PC1:  // PC1
        GPIOC_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOC, 1);
        SET_GPIO_MODE(GPIOC, 1, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOC, 1);
        SET_GPIO_PULL(GPIOC, 1, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_12_AT_PC2:  // PC2
        GPIOC_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOC, 2);
        SET_GPIO_MODE(GPIOC, 2, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOC, 2);
        SET_GPIO_PULL(GPIOC, 2, GPIO_PUPDR_NO_PULL);
        break;
      case ADC123_CH_13_AT_PC3:  // PC3
        GPIOC_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOC, 3);
        SET_GPIO_MODE(GPIOC, 3, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOC, 3);
        SET_GPIO_PULL(GPIOC, 3, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_14_AT_PC4:  // PC4
        GPIOC_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOC, 4);
        SET_GPIO_MODE(GPIOC, 4, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOC, 4);
        SET_GPIO_PULL(GPIOC, 4, GPIO_PUPDR_NO_PULL);
        break;
      case ADC12_CH_15_AT_PC5:  // PC5
        GPIOC_CLK_ENABLE();
        CLR_GPIO_MODE(GPIOC, 5);
        SET_GPIO_MODE(GPIOC, 5, GPIO_MODER_MODE_ANALOG);
        CLR_GPIO_PULL(GPIOC, 5);
        SET_GPIO_PULL(GPIOC, 5, GPIO_PUPDR_NO_PULL);
        break;
      default:
        RetCode = ERR_DEVICE;
        break;
    }
  }
  else
  {
    RetCode = ERR_PARAM_ID;
  }
  return RetCode;
}

/**
 * @brief  Read a number of samples of ADC to a buffer
 * @param  ID: ID of the channel
 * @param  Buffer: Pointer to the location of storage.
 * @param  Number_Of_Samples: Number of Samples.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg OPERATION_RUNNING: The ADC is acquiring the values.
 *             @arg ERR_BUSY: The ADC is being used by another ID.
 *
 */
ReturnCode_t ADC_Read(uint8_t ID, uint32_t *Buffer, uint32_t Number_Of_Samples)
{
  ReturnCode_t RetCode;
  uint32_t ID_ADC, ADC_Hardware, Channel_State;
  ADC_TypeDef  *Instance;

  enum { State_Configuring, State_Converting };

  if(ID <= ADC_MAX_CHANNEL_ID)
  {
    ID_ADC           = ADC_Channel_List[ID].ID_ADC;
    ADC_Hardware     = ADC_Hardware_List[ID_ADC].ADC_Hardware;
    Channel_State    = ADC_Channel_List[ID].State;

    switch(Channel_State)
    {
      case  State_Configuring:
        if(ADC_Driver_Mutex_Try_Lock(ID, &ADC_Channels_Mutex[ID_ADC]) == LOCK_ERROR) // Verify if the driver is free
        {
          return ERR_BUSY; // Some another ID is using the driver.
        }
        else
        {
          switch (ADC_Hardware)
          {
            case ADC_1:
              Instance = ADC1;
              break;
            case ADC_2:
              Instance = ADC2;
              break;
            case ADC_3:
              Instance = ADC3;
              break;
          }
          if (ADC_Channel_List[ID].Channel > ADC12_CH_9_AT_PB1)
          {
            /* Clear the old sample time */
            Instance->SMPR1 &= ~(ADC_SMPR1_SMP10 << (3U * (ADC_Channel_List[ID].Channel - 10U)));
            /* Set the new sample time */
            Instance->SMPR1 |= (ADC_Channel_List[ID].Channel_Sampling_Time << (3U * (ADC_Channel_List[ID].Channel - 10U)));
          }
          else /* ADC_Channel include in ADC_Channel_[0..9] */
          {
            /* Clear the old sample time */
            Instance->SMPR2 &= ~(ADC_SMPR2_SMP0 << (3U * ADC_Channel_List[ID].Channel));
            /* Set the new sample time */
            Instance->SMPR2 |= (ADC_Channel_List[ID].Channel_Sampling_Time << (3U * ADC_Channel_List[ID].Channel));
          }
          /* For Rank 1 to 6 */
          /* Clear the old SQx bits for the selected rank */
          Instance->SQR3 &= ~ADC_SQR3_RK(ADC_SQR3_SQ1, 1);
          /* Set the SQx bits for the selected rank */
          Instance->SQR3 |= ADC_SQR3_RK(ADC_Channel_List[ID].Channel, 1);
          /* Clear regular group conversion flag and overrun flag */
          /* (To ensure of no unknown state from potential previous ADC operations) */
          Instance->SR = ~(ADC_SR_STRT | ADC_SR_EOC | ADC_SR_OVR);
          /* Enable end of conversion interrupt for regular group */
          Instance->CR1 |= (ADC_CR1_EOCIE | ADC_CR1_OVRIE);
          /* Enable the selected ADC software conversion for regular group */
          Instance->CR2 |= ADC_CR2_SWSTART;

          Sample_Counter[ADC_Hardware] = 0;
          Temporary_Buffer[ADC_Hardware] = Buffer;
          Number_Of_Samples_To_Read[ADC_Hardware] = Number_Of_Samples;
          ADC_Channel_List[ID].State = State_Converting;
          RetCode = OPERATION_RUNNING;
        }
        break;

      case  State_Converting:
        RetCode = OPERATION_RUNNING;
        if(Sample_Counter[ADC_Hardware] >= Number_Of_Samples_To_Read[ADC_Hardware])
        {
          ADC_Driver_Mutex_Release(&ADC_Channels_Mutex[ID_ADC]); // Unlock the driver for others ID's
          ADC_Channel_List[ID].State = State_Configuring;
          RetCode = ANSWERED_REQUEST;
        }
        break;
    }
  }
  return RetCode;
}
