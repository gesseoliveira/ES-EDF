/**
 *******************************************************************************
 * @file    ADC.h
 * @author  Peterson Aguiar
 * @brief   ADC module driver.
 * @brief   Version 2019.9.1
 * @brief   Creation Date 07/05/2019
 *
 *          This file provides functions to manage ADC
 *          functionalities:
 *           + Initialization of a ADC
 *           + Setup ADC Channel
 *           + Read ADC Channel
 *
 */

#ifndef DRV_ADC_H_
#define DRV_ADC_H_

#include "returncode.h"
#include "types.h"

#ifndef ADC_MAX_ID
#define ADC_MAX_ID  3
#endif

#ifndef ADC_MAX_CHANNEL_ID
#define ADC_MAX_CHANNEL_ID  15
#endif

extern uint32_t *Temporary_Buffer[ADC_MAX_ID];
extern uint32_t Number_Of_Samples_To_Read[ADC_MAX_ID];
extern uint32_t Sample_Counter[ADC_MAX_ID];

extern uint32_t ADC_Frequency[ADC_MAX_ID];
/**
 * @brief  ADC Clock Hardware enumeration.
 */
typedef enum
{
  ADC_1       = (uint8_t) 0,
  ADC_2       = (uint8_t) 1,
  ADC_3       = (uint8_t) 2
}ADC_hardware_t;

/**
 * @brief  ADC Clock Prescaler enumeration.
 */
typedef enum
{
  ADC_CLOCK_DIV2      = (0x00000000U),
  ADC_CLOCK_DIV4      = (0x1U << 16U),
  ADC_CLOCK_DIV6      = (0x2U << 16U),
  ADC_CLOCK_DIV8      = (0x3U << 16U)
}ADC_Clock_Prescaler_t;

/**
 * @brief  ADC Resolution enumeration.
 */
typedef enum
{
  ADC_RES_12_BITS     = (0x00000000U),
  ADC_RES_10_BITS     = (0x1U << 24U),
  ADC_RES_8_BITS      = (0x2U << 24U),
  ADC_RES_6_BITS      = (0x3U << 24U),
}ADC_Resolution_t;

/**
 * @brief  ADC Configuration structure.
 */
typedef struct
{
  ADC_hardware_t          ADC_Hardware;      /*!< Specifies the ADC Hardware index
                                                  This parameter can be any value of @ref ADC_Hardware_t */
  ADC_Clock_Prescaler_t   Clock_Prescaler;   /*!< Specifies the clock prescaler of ADC
                                                  This parameter can be any value of @ref ADC_Clock_Prescaler_t */
  ADC_Resolution_t        Output_Resolution; /*!< Specifies the resolution of ADC.
                                                  This parameter can be any value of @ref ADC_Resolution_t */
}ADC_Parameters_t;

/**
 * @brief  ADC Channel Sampling Time enumeration.
 */
typedef enum
{
  ADC_SAMPLE_TIME_3CYCLES      = (0x00000000U),
  ADC_SAMPLE_TIME_15CYCLES     = (0x1U),
  ADC_SAMPLE_TIME_28CYCLES     = (0x2U),
  ADC_SAMPLE_TIME_56CYCLES     = (0x3U),
  ADC_SAMPLE_TIME_84CYCLES     = (0x4U),
  ADC_SAMPLE_TIME_112CYCLES    = (0x5U),
  ADC_SAMPLE_TIME_144CYCLES    = (0x6U),
  ADC_SAMPLE_TIME_480CYCLES    = (0x7U),
}ADC_Channel_Sampling_Time_t;

/**
 * @brief  ADC Channel Select enumeration.
 */
typedef enum
{
  ADC123_CH_0_AT_PA0   =  (0x00000000U),
  ADC123_CH_1_AT_PA1   =  (1U),
  ADC123_CH_2_AT_PA2   =  (2U),
  ADC123_CH_3_AT_PA3   =  (3U),
  ADC12_CH_4_AT_PA4    =  (4U),
  ADC12_CH_5_AT_PA5    =  (5U),
  ADC12_CH_6_AT_PA6    =  (6U),
  ADC12_CH_7_AT_PA7    =  (7U),
  ADC12_CH_8_AT_PB0    =  (8U),
  ADC12_CH_9_AT_PB1    =  (9U),
  ADC123_CH_10_AT_PC0  =  (10U),
  ADC123_CH_11_AT_PC1  =  (11U),
  ADC123_CH_12_AT_PC2  =  (12U),
  ADC123_CH_13_AT_PC3  =  (13U),
  ADC12_CH_14_AT_PC4   =  (14U),
  ADC12_CH_15_AT_PC5   =  (15U),
}ADC_Channel_Number_t;

typedef struct
{
  ADC_Channel_Number_t          Channel;                /*!< Specifies the Channel tobe configured and used.
                                                             This parameter can be any value from ADC_CH_0
                                                             to ADC_CH_7 in @ref ADC_Channel_Number_t */
  ADC_Channel_Sampling_Time_t   Channel_Sampling_Time;  /*!< Specifies the sampling time of the selected channel.
                                                             This parameter can be any value in @ref ADC_Channel_Sampling_Time_t */
  ADC_hardware_t                ID_ADC;                 /*!< Specifies the ADC hardware of the selected channel.
                                                             This parameter can be any value in @ref ADC_hardware_t */
  uint32_t                      State;                  /*!< Specifies the State of the channel in the State Machine */
}ADC_Channel_Config_t;

/**
 * @brief  Initialize the internal ADC.
 * @param  ID: ID of the ADC
 * @param  ADC_Configuration: ADC Configuration structure @ref ADC_Parameters_t
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 * @note   This routine will configure the ADC to perform reading on ADC channels using the Interrupt.
 *
 */
ReturnCode_t ADC_Init(uint8_t ID, ADC_Parameters_t ADC_Configuration);

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
ReturnCode_t ADC_Setup_Channel(uint8_t ID_Channel, uint8_t ID_ADC, ADC_Channel_Config_t ADC_Channel_Setup);

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
ReturnCode_t ADC_Read(uint8_t ID_Channel, uint32_t *Buffer, uint32_t Number_Of_Samples);

/**
 * @brief  Deinitialize the internal ADC.
 * @param  ID: ID of the ADC
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 *
 */
ReturnCode_t ADC_Deinit(uint8_t ID);

/**
 * @brief  Reconfigure the internal ADC.
 * @param  ID: ID of the ADC
 * @param  ADC_Configuration: ADC Configuration structure @ref ADC_Parameters_t
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 *
 */
ReturnCode_t ADC_Reconfigure(uint8_t ID, ADC_Parameters_t ADC_Configuration);


#endif /* DRV_ADC_H_ */
