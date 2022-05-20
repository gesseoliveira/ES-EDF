/**
 ******************************************************************************
 * @file    IIC.c
 * @author  Plinio Barbosa da Silva
 * @brief   IIC module driver.
 * @brief   Version 2019.11.2
 * @brief   Creation Date 16/07/2019
 *
 *          This file provides functions to manage IIC
 *          functionalities:
 *           + Initialization of a IIC
 *           + Read data from IIC
 *           + Write data to IIC
 */
#include "stm32f4xx.h"
#include "returncode.h"
#include "sys_cfg_stm32f407.h"
#include "IIC.h"
#include "string.h"
#include "stdlib.h"
#include "stdbool.h"

#define READ_OPERATION	1
#define WRITE_OPERATION	0


#define APB1CLOCK						42000000U

#define I2C_OWNADDRESS1_7BIT             0x00004000U                                /*!< Own address 1 is a 7-bit address.   */
#define I2C_OWNADDRESS1_10BIT            (uint32_t)(I2C_OAR1_ADDMODE | 0x00004000U) /*!< Own address 1 is a 10-bit address.  */


/**
 * @brief  Compute I2C Clock rising time.
 * @param  __FREQRANGE__ This parameter must be a value of peripheral clock (in Mhz).
 * @param  __SPEED__ This parameter must be a value lower than 400kHz (in Hz).
 * @retval Value between Min_Data=0x02 and Max_Data=0x3F
 */
#define I2C_RISE_TIME(__FREQRANGE__, __SPEED__)                    (uint32_t)(((__SPEED__) <= 100000U) ? ((__FREQRANGE__) + 1U) : ((((__FREQRANGE__) * 300U) / 1000U) + 1U))

/**
 * @brief  Compute Speed Fast clock range to a Clock Control Register (I2C_CCR_CCR) value.
 * @param  __PCLK__ This parameter must be a value of peripheral clock (in Hz).
 * @param  __SPEED__ This parameter must be a value between Min_Data=100Khz and Max_Data=400Khz (in Hz).
 * @param  __DUTYCYCLE__ This parameter can be one of the following values:
 *         @arg @ref LL_I2C_DUTYCYCLE_2
 *         @arg @ref LL_I2C_DUTYCYCLE_16_9
 * @retval Value between Min_Data=0x001 and Max_Data=0xFFF
 */
#define I2C_SPEED_FAST_TO_CCR(__PCLK__, __SPEED__, __DUTYCYCLE__)  (uint32_t)(((__DUTYCYCLE__) == 0x00000000U)? \
		(((((__PCLK__) / ((__SPEED__) * 3U)) & I2C_CCR_CCR) == 0U)? 1U:((__PCLK__) / ((__SPEED__) * 3U))) : \
		(((((__PCLK__) / ((__SPEED__) * 25U)) & I2C_CCR_CCR) == 0U)? 1U:((__PCLK__) / ((__SPEED__) * 25U))))
/**
 * @brief  Compute Speed Standard clock range to a Clock Control Register (I2C_CCR_CCR) value.
 * @param  __PCLK__ This parameter must be a value of peripheral clock (in Hz).
 * @param  __SPEED__ This parameter must be a value lower than 100kHz (in Hz).
 * @retval Value between Min_Data=0x004 and Max_Data=0xFFF.
 */
#define I2C_SPEED_STANDARD_TO_CCR(__PCLK__, __SPEED__)             (uint32_t)(((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))


/*****************************************************************************************************************/
/*                                             GPIO ALTERNATIVE FUNCTIONS                                        */
/*                                                                                                               */
/*            Lembre que: AFR[0] -> GPIO0 até GPIO7                                                              */
/*            GPIOD->AFR[0] |= (ALTERNATE_FUNCTION_NUMBER << (4 * GPIO_NUMBER))                                  */
/*            Lembre que: AFR[1] -> GPIO8 até GPIO15                                                             */
/*            GPIOD->AFR[0] |= (ALTERNATE_FUNCTION_NUMBER << (4 * (GPIO_NUMBER - 8)))                            */
/*                                                                                                               */
/*                                                                                                               */
/*                                                                                                               */
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

void I2C_Set_Speed(I2C_TypeDef *I2C_x, uint32_t PeriphClock, uint32_t ClockSpeed, uint32_t DutyCycle);
void IIC_Initialize(uint8_t ID, I2C_TypeDef *IIC_Instance);
void IIC_DeInit(uint8_t ID);

ReturnCode_t IIC_Write(I2C_TypeDef *IIC_Instance, I2C_Handler_t *IIC_Handler, uint8_t Device_Address, uint32_t Register_Address, uint8_t Register_Address_Size, uint8_t *Transmit_Buffer, uint16_t Transmit_Size);
ReturnCode_t IIC_Read(I2C_TypeDef *IIC_Instance, I2C_Handler_t *IIC_Handler, uint8_t Device_Address, uint32_t Register_Address, uint8_t Register_Address_Size, uint8_t *Receive_Buffer, uint16_t Receive_Size);

I2C_Handler_t	I2C1_Handler;
I2C_Handler_t	I2C2_Handler;
I2C_Handler_t	I2C3_Handler;

IIC_Parameters_t	IIC_Cfg_Parameter[IIC_MAX_AVAILABLE_ID];
bool				IIC_Initialization_Status[IIC_MAX_AVAILABLE_ID];

/**
 * @brief  IIC configuration routine.
 * @param  ID ID that should be allocated and configured.
 * @param  IIC_Param to the desired IIC's parameters
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_Init(uint8_t ID, IIC_Parameters_t IIC_Param)
{
	ReturnCode_t	RetCode;

	RetCode = ANSWERED_REQUEST;

	if(ID < IIC_MAX_AVAILABLE_ID)
	{
		if(IIC_Initialization_Status[ID] == false)
		{
			switch (IIC_Param.RouteOption)
			{
			case IIC1_AT_PB7_PB6_SDA_SCL:
				SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

				/* Configure PB6 to I2C1_SCL */
				SET_GPIO_AFRL(GPIOB	, 6, 4);
				SET_GPIO_MODE(GPIOB	, 6, GPIO_MODER_MODE_ALT_FUNC);
				SET_GPIO_PULL(GPIOB	, 6, GPIO_PUPDR_PULL_UP);
				SET_GPIO_SPEED(GPIOB, 6, GPIO_OSPEED_HIGH_SPEED_100MHZ);
				SET_BIT(GPIOB->OTYPER, (1U << 6U));
				/* Configure PB7 to I2C1_SDA */
				SET_GPIO_AFRL(GPIOB	, 7, 4);
				SET_GPIO_MODE(GPIOB	, 7, GPIO_MODER_MODE_ALT_FUNC);
				SET_GPIO_PULL(GPIOB	, 7, GPIO_PUPDR_PULL_UP);
				SET_GPIO_SPEED(GPIOB, 7, GPIO_OSPEED_HIGH_SPEED_100MHZ);
				SET_BIT(GPIOB->OTYPER, (1U << 7U));

				memcpy(&IIC_Cfg_Parameter[ID], &IIC_Param, sizeof(IIC_Parameters_t));

				I2C1_Handler.IIC_Operation_Error_Callback 			= IIC_Cfg_Parameter[ID].IIC_Operation_Error_Callback;
				I2C1_Handler.IIC_Operation_Read_Finished_Callback 	= IIC_Cfg_Parameter[ID].IIC_Operation_Read_Finished_Callback;
				I2C1_Handler.IIC_Operation_Write_Finished_Callback 	= IIC_Cfg_Parameter[ID].IIC_Operation_Write_Finished_Callback;

				IIC_Initialize(ID, I2C1);
				NVIC_SetPriority(I2C1_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
				NVIC_EnableIRQ(I2C1_EV_IRQn);

				NVIC_SetPriority(I2C1_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
				NVIC_EnableIRQ(I2C1_ER_IRQn);

				IIC_Initialization_Status[ID] = true;
				break;
			case IIC2_AT_PB11_PB10_SDA_SCL:
				SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

				/* Configure PB10 to I2C2_SCL */
				SET_GPIO_AFRH(GPIOB	, 10, 4);
				SET_GPIO_MODE(GPIOB	, 10, GPIO_MODER_MODE_ALT_FUNC);
				SET_GPIO_PULL(GPIOB	, 10, GPIO_PUPDR_PULL_UP);
				SET_GPIO_SPEED(GPIOB, 10, GPIO_OSPEED_HIGH_SPEED_100MHZ);
				SET_BIT(GPIOB->OTYPER, (1U << 11U));
				/* Configure PB11 to I2C1_SDA */
				SET_GPIO_AFRH(GPIOB	, 11, 4);
				SET_GPIO_MODE(GPIOB	, 11, GPIO_MODER_MODE_ALT_FUNC);
				SET_GPIO_PULL(GPIOB	, 11, GPIO_PUPDR_PULL_UP);
				SET_GPIO_SPEED(GPIOB, 11, GPIO_OSPEED_HIGH_SPEED_100MHZ);
				SET_BIT(GPIOB->OTYPER, (1U << 11U));

				memcpy(&IIC_Cfg_Parameter[ID], &IIC_Param, sizeof(IIC_Parameters_t));

				I2C2_Handler.IIC_Operation_Error_Callback 			= IIC_Cfg_Parameter[ID].IIC_Operation_Error_Callback;
				I2C2_Handler.IIC_Operation_Read_Finished_Callback 	= IIC_Cfg_Parameter[ID].IIC_Operation_Read_Finished_Callback;
				I2C2_Handler.IIC_Operation_Write_Finished_Callback 	= IIC_Cfg_Parameter[ID].IIC_Operation_Write_Finished_Callback;

				IIC_Initialize(ID, I2C2);
				NVIC_SetPriority(I2C2_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
				NVIC_EnableIRQ(I2C2_EV_IRQn);

				NVIC_SetPriority(I2C2_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
				NVIC_EnableIRQ(I2C2_ER_IRQn);

				IIC_Initialization_Status[ID] = true;
				break;

			case IIC3_AT_PC9_PA8_SDA_SCL:
				SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
				SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

				/* Configure PA8 to I2C3_SCL */
				SET_GPIO_AFRH(GPIOA	, 8, 4);
				SET_GPIO_MODE(GPIOA	, 8, GPIO_MODER_MODE_ALT_FUNC);
				SET_GPIO_PULL(GPIOA	, 8, GPIO_PUPDR_PULL_UP);
				SET_GPIO_SPEED(GPIOA, 8, GPIO_OSPEED_HIGH_SPEED_100MHZ);
				SET_BIT(GPIOA->OTYPER, (1U << 8U));
				/* Configure PC9 to I2C1_SDA */
				SET_GPIO_AFRH(GPIOC	, 9, 4);
				SET_GPIO_MODE(GPIOC	, 9, GPIO_MODER_MODE_ALT_FUNC);
				SET_GPIO_PULL(GPIOC	, 9, GPIO_PUPDR_PULL_UP);
				SET_GPIO_SPEED(GPIOC, 9, GPIO_OSPEED_HIGH_SPEED_100MHZ);
				SET_BIT(GPIOC->OTYPER, (1U << 9U));

				memcpy(&IIC_Cfg_Parameter[ID], &IIC_Param, sizeof(IIC_Parameters_t));

				I2C3_Handler.IIC_Operation_Error_Callback 			= IIC_Cfg_Parameter[ID].IIC_Operation_Error_Callback;
				I2C3_Handler.IIC_Operation_Read_Finished_Callback 	= IIC_Cfg_Parameter[ID].IIC_Operation_Read_Finished_Callback;
				I2C3_Handler.IIC_Operation_Write_Finished_Callback 	= IIC_Cfg_Parameter[ID].IIC_Operation_Write_Finished_Callback;

				IIC_Initialize(ID, I2C3);
				NVIC_SetPriority(I2C3_EV_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
				NVIC_EnableIRQ(I2C3_EV_IRQn);

				NVIC_SetPriority(I2C3_ER_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
				NVIC_EnableIRQ(I2C3_ER_IRQn);

				IIC_Initialization_Status[ID] = true;
				break;

			default:
				RetCode = ERR_PARAM_CONFIG;
				break;
			}
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
 * @brief  Basic receive data routine.
 * @param  ID IIC ID number.
 * @param  RecBuffer Pointer to store the received data.
 * @param  RecLength Maximum amount of data to receive.
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_ReceiveData(uint8_t ID, uint32_t Register, uint8_t *RecBuffer, uint16_t RecLength)
{
	ReturnCode_t	RetCode;
	if(IIC_Initialization_Status[ID] == true)
	{
		switch (IIC_Cfg_Parameter[ID].RouteOption)
		{
		case IIC1_AT_PB7_PB6_SDA_SCL:
			RetCode	=	IIC_Read(I2C1, &I2C1_Handler, IIC_Cfg_Parameter[ID].Address, Register, IIC_Cfg_Parameter[ID].DataAddrSize, RecBuffer, RecLength);
			break;
		case IIC2_AT_PB11_PB10_SDA_SCL:
			RetCode	=	IIC_Read(I2C2, &I2C2_Handler, IIC_Cfg_Parameter[ID].Address, Register, IIC_Cfg_Parameter[ID].DataAddrSize, RecBuffer, RecLength);
			break;
		case IIC3_AT_PC9_PA8_SDA_SCL:
			RetCode	=	IIC_Read(I2C3, &I2C3_Handler, IIC_Cfg_Parameter[ID].Address, Register, IIC_Cfg_Parameter[ID].DataAddrSize, RecBuffer, RecLength);
			break;
		default:
			RetCode = ERR_PARAM_CONFIG;
			break;
		}
	}
	else
	{
		return ERR_DISABLED;
	}

	return RetCode;
}

/**
 * @brief  Basic send data routine.
 * @param  ID IIC ID number.
 * @param  SendBuffer Pointer to where the data is.
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_SendData(uint8_t ID, uint32_t Register, uint8_t *SendBuffer, uint16_t SendLength)
{
	ReturnCode_t	RetCode;
	if(IIC_Initialization_Status[ID] == true)
	{
		switch (IIC_Cfg_Parameter[ID].RouteOption)
		{
		case IIC1_AT_PB7_PB6_SDA_SCL:
			RetCode	=	IIC_Write(I2C1, &I2C1_Handler, IIC_Cfg_Parameter[ID].Address, Register, IIC_Cfg_Parameter[ID].DataAddrSize, SendBuffer, SendLength);
			break;
		case IIC2_AT_PB11_PB10_SDA_SCL:
			RetCode	=	IIC_Write(I2C2, &I2C2_Handler, IIC_Cfg_Parameter[ID].Address, Register, IIC_Cfg_Parameter[ID].DataAddrSize, SendBuffer, SendLength);
			break;
		case IIC3_AT_PC9_PA8_SDA_SCL:
			RetCode	=	IIC_Write(I2C3, &I2C3_Handler, IIC_Cfg_Parameter[ID].Address, Register, IIC_Cfg_Parameter[ID].DataAddrSize, SendBuffer, SendLength);
			break;
		default:
			RetCode = ERR_PARAM_CONFIG;
			break;
		}
	}
	else
	{
		return ERR_DISABLED;
	}

	return RetCode;
}

ReturnCode_t IIC_Read(I2C_TypeDef *IIC_Instance, I2C_Handler_t *IIC_Handler, uint8_t Device_Address, uint32_t Register_Address, uint8_t Register_Address_Size, uint8_t *Receive_Buffer, uint16_t Receive_Size)
{
	uint8_t Dummy_8bit;
	switch (IIC_Handler->State_Machine_IIC) {
	case STATE_IIC_Prepare_Operation:
		if((Register_Address_Size == 0) || (Receive_Size == 0) || (Receive_Buffer == 0))
			goto return_err_device;
		IIC_Handler->Dev_Address 			= Device_Address;
		IIC_Handler->pRxBuffer 				= Receive_Buffer;
		IIC_Handler->RxSize    				= Receive_Size;
		IIC_Handler->TxSize    				= 0;
		IIC_Handler->Register_Address		= Register_Address;
		IIC_Handler->Register_Address_Size	= Register_Address_Size;
		IIC_Handler->Status					= IIC_BUSY_WRITE;
		IIC_Handler->Mode					= IIC_WRITE;
		IIC_Handler->Op_Type					= IIC_MEM_READ;
		Dummy_8bit = IIC_Instance->DR;
		(void)Dummy_8bit;
		SET_BIT(IIC_Instance->CR1, I2C_CR1_START); // Generate a start bit
		SET_BIT(IIC_Instance->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);

		IIC_Handler->State_Machine_IIC = STATE_IIC_Wait_Finish;
		break;
	case STATE_IIC_Wait_Finish:
		if(IIC_Handler->Status == IIC_IDLE)
		{
			IIC_Handler->State_Machine_IIC = STATE_IIC_Prepare_Operation;
			goto return_answered_request;
		} else if(IIC_Handler->Status == IIC_ERROR)
		{
			IIC_Handler->Status = IIC_IDLE;
			IIC_Handler->State_Machine_IIC = STATE_IIC_Prepare_Operation;
			//SET_BIT(IIC_Instance->CR1, I2C_CR1_STOP);
			goto return_err_device;
		}
		break;
	}

	return OPERATION_RUNNING;
	return_err_device:
	//CLEAR_BIT(IIC_Instance->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
	return ERR_DEVICE;
	return_answered_request:
	//CLEAR_BIT(IIC_Instance->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
	return ANSWERED_REQUEST;
}

ReturnCode_t IIC_Write(I2C_TypeDef *IIC_Instance, I2C_Handler_t *IIC_Handler, uint8_t Device_Address, uint32_t Register_Address, uint8_t Register_Address_Size, uint8_t *Transmit_Buffer, uint16_t Transmit_Size)
{
	uint8_t Dummy_8bit;
	switch (IIC_Handler->State_Machine_IIC) {
	case STATE_IIC_Prepare_Operation:
		if((Register_Address_Size == 0) || (Transmit_Size == 0) || (Transmit_Buffer == 0))
			goto return_err_device;
		IIC_Handler->Dev_Address 			= Device_Address;
		IIC_Handler->pTxBuffer 				= Transmit_Buffer;
		IIC_Handler->TxSize    				= Transmit_Size;
		IIC_Handler->Register_Address		= Register_Address;
		IIC_Handler->Register_Address_Size	= Register_Address_Size;
		IIC_Handler->Status					= IIC_BUSY_WRITE;
		IIC_Handler->Mode					= IIC_WRITE;
		IIC_Handler->Op_Type				= IIC_MEM_WRITE;
		Dummy_8bit = IIC_Instance->DR;
		(void)Dummy_8bit;
		SET_BIT(IIC_Instance->CR1, I2C_CR1_START); // Generate a start bit
		SET_BIT(IIC_Instance->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);

		IIC_Handler->State_Machine_IIC = STATE_IIC_Wait_Finish;
		break;
	case STATE_IIC_Wait_Finish:
		if(IIC_Handler->Status == IIC_IDLE)
		{
			IIC_Handler->State_Machine_IIC = STATE_IIC_Prepare_Operation;
			goto return_answered_request;
		} else if(IIC_Handler->Status == IIC_ERROR)
		{
			IIC_Handler->Status = IIC_IDLE;
			IIC_Handler->State_Machine_IIC = STATE_IIC_Prepare_Operation;
			//SET_BIT(IIC_Instance->CR1, I2C_CR1_STOP);
			goto return_err_device;
		}
		break;
	}

	return OPERATION_RUNNING;
	return_err_device:
	//CLEAR_BIT(IIC_Instance->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
	return ERR_DEVICE;
	return_answered_request:
	//CLEAR_BIT(IIC_Instance->CR2, I2C_CR2_ITEVTEN | I2C_CR2_ITBUFEN | I2C_CR2_ITERREN);
	return ANSWERED_REQUEST;
}
/**
 * @brief Reconfigure IIC hardware with the specified parameters
 * @param  ID IIC ID number.
 * @param  IIC_Config Pointer to the desired IIC's parameters
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_Reconfigure(uint8_t ID, IIC_Parameters_t IIC_Param)
{
	ReturnCode_t	RetCode;
	if(ID < IIC_MAX_AVAILABLE_ID)
	{
		if(IIC_Initialization_Status[ID] == false)
		{
			RetCode = IIC_Init(ID, IIC_Param);
		}
		else if(IIC_Initialization_Status[ID] == true)
		{
			IIC_DeInit(ID);
			RetCode = IIC_Init(ID, IIC_Param);
		}
	}
	else
	{
		RetCode = ERR_PARAM_ID;
	}
	return RetCode;
}

void IIC_DeInit(uint8_t ID)
{
	if(IIC_Initialization_Status[ID] == true)
	{
		switch (IIC_Cfg_Parameter[ID].RouteOption)
		{
		case IIC1_AT_PB7_PB6_SDA_SCL:
			CLEAR_BIT(I2C1->CR1, I2C_CR1_PE);
			break;
		case IIC2_AT_PB11_PB10_SDA_SCL:
			CLEAR_BIT(I2C2->CR1, I2C_CR1_PE);
			break;
		case IIC3_AT_PC9_PA8_SDA_SCL:
			CLEAR_BIT(I2C3->CR1, I2C_CR1_PE);
			break;
		default:
			break;
		}
	}
}



void IIC_Initialize(uint8_t ID, I2C_TypeDef *IIC_Instance)
{
	// Enable I2C peripheral clock
	if(IIC_Instance == I2C1)
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);
	else if (IIC_Instance == I2C2)
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C2EN);
	else if (IIC_Instance == I2C3)
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C3EN);

	// Disable  acknowledge on Own Address2 match address.
	CLEAR_BIT(IIC_Instance->OAR2, I2C_OAR2_ENDUAL);
	// Disable General Call
	CLEAR_BIT(IIC_Instance->CR1, I2C_CR1_ENGC);
	// Enable Clock stretching.
	CLEAR_BIT(IIC_Instance->CR1, I2C_CR1_NOSTRETCH);

	/* Disable the selected I2Cx Peripheral */
	CLEAR_BIT(IIC_Instance->CR1, I2C_CR1_PE);


	switch (IIC_Cfg_Parameter[ID].BaudRate)
	{
	case IIC_100kbps:
		I2C_Set_Speed(IIC_Instance, APB1CLOCK, 100000U, 0x00000000U); // Duty set to 2
		break;
	case IIC_400kbps:
		I2C_Set_Speed(IIC_Instance, APB1CLOCK, 400000U, I2C_CCR_DUTY); // Duty set to 16/9
		break;
	}
	//I2C_Set_Speed(IIC_x, 82000000U, 100000U, I2C_CCR_DUTY); // Duty 16/9

	switch (IIC_Cfg_Parameter[ID].AddrSize)
	{
	case ADDR_SIZE_7:
		// 7 Bit address mode
		MODIFY_REG(IIC_Instance->OAR1, I2C_OAR1_ADD0 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD8_9 | I2C_OAR1_ADDMODE, 0 | I2C_OWNADDRESS1_7BIT);
		break;
	case ADDR_SIZE_10:
		// 10 Bit address mode
		MODIFY_REG(IIC_Instance->OAR1, I2C_OAR1_ADD0 | I2C_OAR1_ADD1_7 | I2C_OAR1_ADD8_9 | I2C_OAR1_ADDMODE, 0 | I2C_OWNADDRESS1_10BIT);
		break;
	}

	// Set mode to I2C
	MODIFY_REG(IIC_Instance->CR1, I2C_CR1_SMBUS | I2C_CR1_SMBTYPE | I2C_CR1_ENARP, 0x00000000U);

	// Enable I2C Peripheral
	SET_BIT(IIC_Instance->CR1, I2C_CR1_PE);

	// Send acknowledge after received data
	MODIFY_REG(IIC_Instance->CR1, I2C_CR1_ACK, I2C_CR1_ACK);

	// Set own address2
	MODIFY_REG(IIC_Instance->OAR2, I2C_OAR2_ADD2, 0);

}

void I2C_Set_Speed(I2C_TypeDef *I2C_x, uint32_t PeriphClock, uint32_t ClockSpeed, uint32_t DutyCycle)
{
	register uint32_t freqrange = 0x0U;
	register uint32_t clockconfig = 0x0U;

	/* Compute frequency range */
	freqrange = (uint32_t)((PeriphClock)/1000000U);

	/* Configure I2Cx: Frequency range register */
	MODIFY_REG(I2C_x->CR2, I2C_CR2_FREQ, freqrange);

	/* Configure I2Cx: Rise Time register */
	MODIFY_REG(I2C_x->TRISE, I2C_TRISE_TRISE, I2C_RISE_TIME(freqrange, ClockSpeed));

	/* Configure Speed mode, Duty Cycle and Clock control register value */
	if (ClockSpeed > 100000U)
	{
		/* Set Speed mode at fast and duty cycle for Clock Speed request in fast clock range */
		clockconfig = I2C_CCR_FS | I2C_SPEED_FAST_TO_CCR(PeriphClock, ClockSpeed, DutyCycle) | DutyCycle;
	}
	else
	{
		/* Set Speed mode at standard for Clock Speed request in standard clock range */
		clockconfig = 0x00000000U | I2C_SPEED_STANDARD_TO_CCR(PeriphClock, ClockSpeed);
	}

	/* Configure I2Cx: Clock control register */
	MODIFY_REG(I2C_x->CCR, (I2C_CCR_FS | I2C_CCR_DUTY | I2C_CCR_CCR), clockconfig);
}
