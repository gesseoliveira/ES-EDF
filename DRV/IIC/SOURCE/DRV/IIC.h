/**
 ******************************************************************************
 * @file    IIC.h
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

#ifndef IIC_H_
#define IIC_H_

#include "stdint.h"
#include "returncode.h"
#include "SETUP.h"

#ifndef IIC_MAX_AVAILABLE_ID
#define IIC_MAX_AVAILABLE_ID 3
#endif

typedef enum
{
	IIC_IDLE,
	IIC_BUSY_WRITE,
	IIC_BUSY_READ,
	IIC_ERROR,
}I2C_Status_t;

typedef enum
{
	IIC_WRITE,
	IIC_READ,
}I2C_Mode_t;

typedef enum
{
	IIC_MEM_WRITE,
	IIC_MEM_READ,
}I2C_Op_Type_t;

typedef enum
{
	STATE_IIC_Prepare_Operation,
	STATE_IIC_Wait_Finish
} State_IIC_t;
typedef void (*IIC_Operation_Callback_t)(void);

typedef struct
{
	uint8_t 			*pTxBuffer;
	uint8_t 			*pRxBuffer;
	uint16_t 			TxSize;
	uint16_t 			RxSize;
	uint8_t				Register_Address_Size;
	uint32_t			Register_Address;
	uint8_t 			Dev_Address;
	I2C_Status_t		Status;
	I2C_Mode_t			Mode;
	I2C_Op_Type_t		Op_Type;
	State_IIC_t			State_Machine_IIC;
	IIC_Operation_Callback_t	IIC_Operation_Error_Callback;
	IIC_Operation_Callback_t	IIC_Operation_Write_Finished_Callback;
	IIC_Operation_Callback_t	IIC_Operation_Read_Finished_Callback;
}I2C_Handler_t;

extern I2C_Handler_t	I2C1_Handler;
extern I2C_Handler_t	I2C2_Handler;
extern I2C_Handler_t	I2C3_Handler;

typedef enum
{
  IIC1_AT_PB7_PB6_SDA_SCL,
  IIC2_AT_PB11_PB10_SDA_SCL,
  IIC3_AT_PC9_PA8_SDA_SCL,
} RoutedPortOptions_t;

typedef enum
{
	ADDR_SIZE_7,
	ADDR_SIZE_10,
} IIC_AddressSize_t;

typedef enum
{
	IIC_DATA_ADDR_8BITS 	= 1,
	IIC_DATA_ADDR_16BITS	= 2,
	IIC_DATA_ADDR_24BITS	= 3,
	IIC_DATA_ADDR_32BITS	= 4,
} IIC_DataAddrSize_t;

typedef enum
{
	IIC_400kbps = 0,
	IIC_100kbps,
} IIC_BaudRate_t;

typedef enum
{
	IIC_PULLUP_DISABLED = 0,
	IIC_PULLUP_ENABLED,
} IIC_PullUp_t;

typedef struct
{
	RoutedPortOptions_t		RouteOption;
	IIC_AddressSize_t       AddrSize;
	uint16_t              	Address;
	IIC_BaudRate_t          BaudRate;
	IIC_PullUp_t            PullUpSelect;
	IIC_DataAddrSize_t      	DataAddrSize;
	IIC_Operation_Callback_t	IIC_Operation_Error_Callback;
	IIC_Operation_Callback_t	IIC_Operation_Write_Finished_Callback;
	IIC_Operation_Callback_t	IIC_Operation_Read_Finished_Callback;
}IIC_Parameters_t;


/**
 * @brief  IIC configuration routine.
 * @param  ID ID that should be allocated and configured.
 * @param  IIC_Param to the desired IIC's parameters
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_Init(uint8_t ID, IIC_Parameters_t IIC_Param);

/**
 * @brief  Basic send data routine.
 * @param  ID IIC ID number.
 * @param  SendBuffer Pointer to where the data is.
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_SendData(uint8_t ID, uint32_t Register, uint8_t *SendBuffer, uint16_t SendLength);

/**
 * @brief  Basic receive data routine.
 * @param  ID IIC ID number.
 * @param  RecBuffer Pointer to store the received data.
 * @param  RecLength Maximum amount of data to receive.
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_ReceiveData(uint8_t ID, uint32_t Register, uint8_t *RecBuffer, uint16_t RecLength);

/**
 * @brief Reconfigure IIC hardware with the specified parameters
 * @param  ID IIC ID number.
 * @param  IIC_Config Pointer to the desired IIC's parameters
 * @retval ReturnCode_t
 */
ReturnCode_t IIC_Reconfigure(uint8_t ID, IIC_Parameters_t IIC_Param);

#endif /* IIC_H_ */
