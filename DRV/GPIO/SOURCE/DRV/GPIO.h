/**

  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================

	...

int main(void)
{
	int i = 0;
	uint8_t GPIO_Input_Value;
	GPIO_Parameters_t GPIO_Configuration;



	HAL_Init();

	SystemClock_Config();

	GPIO_Configuration.DataDirection = GPIO_MODE_IN_NO_PULL;
	GPIO_Configuration.GPIO_Pin 	= 0;
	GPIO_Configuration.GPIO_Port	= GPIOA;

	GPIO_Config(BUTTON_ID, GPIO_Configuration);

	GPIO_Configuration.DataDirection= GPIO_MODE_OUT_PP;
	GPIO_Configuration.GPIO_Pin 	= 8;
	GPIO_Configuration.GPIO_Port	= GPIOC;
	GPIO_Config(LED1_ID, GPIO_Configuration);

	GPIO_Configuration.DataDirection= GPIO_MODE_OUT_PP;
	GPIO_Configuration.GPIO_Pin 	= 9;
	GPIO_Configuration.GPIO_Port	= GPIOC;
	GPIO_Config(LED2_ID, GPIO_Configuration);

	while (1)
	{
		GPIO_ReadInput(BUTTON_ID, &GPIO_Input_Value);
		if(GPIO_Input_Value == 1)
		{
			GPIO_SetOutput(LED2_ID);
		}
		else
		{
			GPIO_ClearOutput(LED2_ID);
		}

		GPIO_ToggleOutput(LED1_ID);
		i++;
		HAL_Delay(100);
	}
	return 0;
}
	...

  @endverbatim
 */

#ifndef __GPIO_H__
#define __GPIO_H__

#include "returncode.h"
#include "stdint.h"
#include "SETUP.h"

#define GPIO_NUMBER           ((uint32_t)16U)




#ifndef GPIO_MAX_AVAILABLE_ID /* It may be defined on Setup.h file */
/**
 * Max number of of GPIO's.
 */
#define GPIO_MAX_AVAILABLE_ID      32
#endif

/**
 * @brief  GPIO Port enumeration.
 */
typedef enum
{
  GPIO_PORT_A,      /*!< PORT_A       */
  GPIO_PORT_B,      /*!< PORT_B       */
  GPIO_PORT_C,      /*!< PORT_C       */
  GPIO_PORT_D,      /*!< PORT_D       */
  GPIO_PORT_E,      /*!< PORT_E       */
  GPIO_PORT_F,      /*!< PORT_F       */
  GPIO_PORT_G,      /*!< PORT_G       */
  GPIO_PORT_H,      /*!< PORT_G       */
}GPIO_Port_t;

/**
 * @brief  GPIO Pin number enumeration.
 */
typedef enum
{
  GPIO_PIN_0_ = 0,
  GPIO_PIN_1_,
  GPIO_PIN_2_,
  GPIO_PIN_3_,
  GPIO_PIN_4_,
  GPIO_PIN_5_,
  GPIO_PIN_6_,
  GPIO_PIN_7_,
  GPIO_PIN_8_,
  GPIO_PIN_9_,
  GPIO_PIN_10_,
  GPIO_PIN_11_,
  GPIO_PIN_12_,
  GPIO_PIN_13_,
  GPIO_PIN_14_,
  GPIO_PIN_15_
} GPIO_Pin_list_t;
/**
 * @}
 */

/**
 * @brief  GPIO Direction configuration enumeration.
 */
typedef enum
{
  GPIO_MODE_IN_PULL_UP   = (0x00000000U),   	 /*!< Input Pull UP Mode                    */
  GPIO_MODE_IN_PULL_DOWN,   				     /*!< Input Pull Down Mode                  */
  GPIO_MODE_IN_NO_PULL,   					 /*!< Input Floating Mode                   */
  GPIO_MODE_OUT_PP,   						 /*!< Output Push Pull Mode                 */
  GPIO_MODE_OUT_OD,   						 /*!< Output Open Drain Mode                */
  GPIO_MODE_ANA								 /*!< Analog Input Mode                     */
}GPIO_Direction_t;
/**
 * @}
 */

/**
 * @brief  GPIO Speed configuration enumeration.
 */
typedef enum
{
  GPIO_SPEED_PIN_SLOW      =(0x00000000U), /*!< max: 400 KHz, please refer to the product datasheet */
  GPIO_SPEED_PIN_MEDIUM    =(0x00000001U), /*!< max: 1 MHz to 2 MHz, please refer to the product datasheet */
  GPIO_SPEED_PIN_HIGH      =(0x00000002U), /*!< max: 2 MHz to 10 MHz, please refer to the product datasheet */
  GPIO_SPEED_PIN_VERY_HIGH =(0x00000003U)  /*!< max: 8 MHz to 50 MHz, please refer to the product datasheet */

}GPIO_Speed_t;
/**
 * @}
 */

/**
 * @brief  GPIO Configuration Structure.
 */
typedef struct
{
  GPIO_Port_t   			GPIO_Port;		/*!< Specifies the GPIO port to be used. */
  GPIO_Pin_list_t			GPIO_Pin;		/*!< Specifies the GPIO pin to be used. */
  GPIO_Direction_t  	GPIO_Direction; /*!< Specifies the pin direction @ref GPIO_Direction_t. */
  GPIO_Speed_t				GPIO_Speed;		/*!< Specifies the pin speed @ref GPIO_Speed_t. */
} GPIO_Parameters_t;
/**
 * @}
 */

/**
 * @brief  GPIO State enumeration.
 */
typedef enum
{
  GPIO_CLEAR = 0, /*!< Specifies the GPIO pin is Set. */
  GPIO_SET   = 1	/*!< Specifies the GPIO is Clear. */
}GPIO_State_t;
/**
 * @}
 */

/**
 * @brief  Pin configuration routine.
 * @param  ID : ID that should be allocated and configured.
 * @param  Parameter : Desired pin's configuration parameters. See @ref GPIO_Parameters_t
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *			   @arg ANSWERED_REQUEST: All ok, GPIO is configured
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_Init(uint8_t ID, GPIO_Parameters_t Parameter);

/**
 * @brief  Output set routine.
 * @param  ID : ID whose pin's should be set.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *			   @arg ANSWERED_REQUEST: All ok, pin's level has been set.
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_SetOutput(uint8_t ID);

/**
 * @brief  Output clear routine.
 * @param  ID : ID whose pin's should be cleared.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *			   @arg ANSWERED_REQUEST: All ok, pin's level has been cleared.
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_ClearOutput(uint8_t ID);

/**
 * @brief  Read pin routine.
 * @param  ID : ID whose pin's should be read.
 * @param  InputValue: Pointer to a variable where the read result will be stored.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *			   @arg ANSWERED_REQUEST: All ok, pin's level has been read.
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_ReadInput(uint8_t ID, uint8_t *InputValue);

/**
 * @brief  Output toggle routine.
 * @param  ID : ID whose pin's should be toggled.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *			   @arg ANSWERED_REQUEST: All ok, pin's level has been toggled.
 *             @arg Else: Some error happened.
 */
ReturnCode_t  GPIO_ToggleOutput(uint8_t ID);

/**
 * @brief  Set the GPIO to reset default values
 * @param  ID GPIO ID number.
 * @retval ReturnCode_t
 */
ReturnCode_t GPIO_DeInit(uint8_t ID);

/**
 * @brief  Reconfigure GPIO hardware with the specified parameters
 * @param  ID GPIO ID number.
 * @param  GPIO_Config Pointer to the desired GPIO's parameters
 * @retval ReturnCode_t
 */
ReturnCode_t GPIO_Reconfigure(uint8_t ID, GPIO_Parameters_t Parameter);

#endif
