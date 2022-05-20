/**
 ******************************************************************************
 * @file    main.c
 * @author  Plinio Barbosa da Silva
 * @version V1.0
 * @brief   Default main function.
 ******************************************************************************
 */

#include "SPI.h"
#include "stm32f4xx.h"
#include "returncode.h"
#include "sys_cfg_stm32f407.h"

SPI_Parameters SPI_Configuration;

uint8_t TXData[] = {0x8F, 0x11, 0x22, 0x33, 0x44};
uint8_t RXData[10];

int main(void)
{
	// Initialize peripheral and power interface clock
	Sys_Enable_Peripherals_Clock();
	// Set the CPU clock and bus clock to 80MHz
	Sys_Configure_Clock_168MHz();

	SPI_Configuration.SelectedPort   		= SPI1_AT_PA5_PA6_PA7_CLK_MISO_MOSI;
	SPI_Configuration.BaudRate       		= SPI_BAUD_PRESCALLER_32;
	SPI_Configuration.Clock_Phase          	= PHA_LEADING_EDGE;
	SPI_Configuration.Clock_Polarity       	= POL_ACTIVE_HIGH;
	SPI_Configuration.ChipSelectPolarity 	= POL_ACTIVE_LOW;
	SPI_Configuration.MosiValueOnReads    	= 0x00;
	SPI_Configuration.ChipSelectPin  		= 3;
	SPI_Configuration.ChipSelectPort 		= SPI_CS_GPIO_PORT_E;
	SPI_Init(0, SPI_Configuration);

	SPI_Configuration.SelectedPort   		= SPI3_AT_PC10_PC11_PB5_CLK_MISO_MOSI;
	SPI_Configuration.BaudRate       		= SPI_BAUD_PRESCALLER_256;
	SPI_Configuration.Clock_Phase          	= PHA_LEADING_EDGE;
	SPI_Configuration.Clock_Polarity        = POL_ACTIVE_HIGH;
	SPI_Configuration.ChipSelectPolarity 	= POL_ACTIVE_LOW;
	SPI_Configuration.MosiValueOnReads      = 0x00;
	SPI_Configuration.ChipSelectPin  		= 1;
	SPI_Configuration.ChipSelectPort 		= SPI_CS_GPIO_PORT_C;
	SPI_Init(1, SPI_Configuration);


	for(;;)
	{
		if(SPI_SendData(0, TXData, 5) == ANSWERED_REQUEST)
			__asm("NOP");
		if(SPI_SendData(1, TXData, 5) == ANSWERED_REQUEST)
			__asm("NOP");
//		if(SPI_ReceiveData(0, RXData, 5) == ANSWERED_REQUEST)
//			__asm("NOP");
//		if(SPI_TransmitReceiveData(0, TXData, RXData, 5) == ANSWERED_REQUEST)
//			__asm("NOP");
		//for(uint32_t tmp = 0x1FF;tmp > 0;tmp --);
//		if(SPI_ReceiveData(1, TXData, 2, RXData, 10) == ANSWERED_REQUEST)
//		{
//			__asm("NOP");
//		}
	}
}
