/**
 ******************************************************************************
 * @file    main.c
 * @author  Plinio Barbosa da Silva
 * @version  2019.11.2
 * @brief   Default main function.
 ******************************************************************************
 */

#include "IIC.h"
#include "stm32f4xx.h"
#include "returncode.h"
#include "sys_cfg_stm32f407.h"
#include "string.h"
#include "stdbool.h"

//#define USE_IIC_CALLBACKS

IIC_Parameters_t IIC_Configuration;

uint8_t Memory_String_Buffer[] = {"SENAI - CIMATEC HELLO WORLD!"};
uint8_t Memory_Contents_Buffer[32];

enum
{
	STATE_Read_Memory,
	STATE_Check_Content,
	STATE_Program_Memory,
	STATE_Sleep,
	STATE_Error,
}EEPROM_24C32_State_Machine = STATE_Read_Memory;

void I2C1_Error_CB(void);
void I2C1_Write_Ok_CB(void);
void I2C1_Read_Ok_CB(void);

#ifdef	USE_IIC_CALLBACKS
bool Send_Flag = false;
bool Receive_Flag = false;
#else
ReturnCode_t	Ret_Code;
#endif

int main(void)
{
	// Initialize peripheral and power interface clock
	Sys_Enable_Peripherals_Clock();
	// Set the CPU clock and bus clock to 80MHz
	Sys_Configure_Clock_168MHz();

	IIC_Configuration.RouteOption	= IIC1_AT_PB7_PB6_SDA_SCL;
	IIC_Configuration.AddrSize 		= ADDR_SIZE_7;
	IIC_Configuration.Address		= 0xA0; 									//IIC bus address of device
	IIC_Configuration.BaudRate		= IIC_100kbps;
	IIC_Configuration.PullUpSelect	= IIC_PULLUP_ENABLED;
	IIC_Configuration.DataAddrSize	= IIC_DATA_ADDR_16BITS;						// Internal address size of the device
#ifdef USE_IIC_CALLBACKS
	IIC_Configuration.IIC_Operation_Error_Callback = I2C1_Error_CB;				// Callback used to detect error on Read/Write
	IIC_Configuration.IIC_Operation_Read_Finished_Callback = I2C1_Read_Ok_CB;	// Callback used to detect when the read operation is finished
	IIC_Configuration.IIC_Operation_Write_Finished_Callback = I2C1_Write_Ok_CB; // Callback used to detect when the write operation is finished
#endif
	IIC_Init(0, IIC_Configuration);

	for(;;)
	{

#ifdef	USE_IIC_CALLBACKS

		IIC_SendData(0, 0x0000, Memory_String_Buffer, sizeof(Memory_String_Buffer)); // A função so precisa ser chamada uma vez. O status da recepção/tranmissão é refletido pela chamada dos callbacks
		do
		{
		}while( Send_Flag != true ); // Este é setado na função de callback "I2C1_Write_Ok_CB()"
		Send_Flag = true;
		for(uint32_t i = 0x1FFFF; i > 0; i--);

		IIC_ReceiveData(0, 0x0000, Memory_Contents_Buffer, sizeof(Memory_String_Buffer));// A função so precisa ser chamada uma vez. O status da recepção/tranmissão é refletido pela chamada dos callbacks
		do
		{
		}while( Receive_Flag != true ); // Este é setado na função de callback "I2C1_Read_Ok_CB()"
		Receive_Flag = false;
		for(uint32_t i = 0xFFFFF; i > 0; i--);

#else
		/*
		 * Exemplo de uso das funcoes IIC sem a utilização dos Callbacks.
		 *
		 */
		do
		{
			Ret_Code = IIC_SendData(0, 0x0000, Memory_String_Buffer, sizeof(Memory_String_Buffer)); // A função deve ser chamada periodicamente para saber o status da transmissão/recepção
		}while( Ret_Code != ANSWERED_REQUEST );

		for(uint32_t i = 0xFFFF; i > 0; i--);

		do
		{
			Ret_Code = IIC_ReceiveData(0, 0x0000, Memory_Contents_Buffer, sizeof(Memory_String_Buffer)); // A função deve ser chamada periodicamente para saber o status da transmissão/recepção
		}while( Ret_Code != ANSWERED_REQUEST );
		for(uint32_t i = 0xFFFFF; i > 0; i--);
#endif
	}
}

#ifdef	USE_IIC_CALLBACKS
void I2C1_Error_CB(void)
{
	__asm("BKPT 255");	// Ocorreu algum erro durante a leitura ou escrita do dispositivo no barramento.
}
void I2C1_Write_Ok_CB(void)
{
	Send_Flag = true;
}
void I2C1_Read_Ok_CB(void)
{
	Receive_Flag = true;
}
#endif
