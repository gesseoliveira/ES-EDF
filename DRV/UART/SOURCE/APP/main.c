/**
 ******************************************************************************
 * @file    main.c
 * @author  Plinio Barbosa da Silva
 * @version V1.0
 * @brief   Default main function.
 ******************************************************************************
 */

#include "UART.h"
#include "stm32f4xx.h"
#include "returncode.h"
#include "sys_cfg_stm32f407.h"
#include "macros.h"

uint8_t TXData[] = {"Hello World!!!\n\t"};
uint8_t RXData[20];
uint16_t RXSize;

int y = 0;
uint16_t t=12333;
float  ex_t;

int main(void)
{
	ReturnCode_t UARTReturn;
	UART_Config_t parameters;
	static enum { State_Receiving, State_Sending } UART_APP_State_Machine = State_Sending;

	// Initialize peripheral and power interface clock
	Sys_Enable_Peripherals_Clock();
	// Set the CPU clock and bus clock to 80MHz
	Sys_Configure_Clock_168MHz();

	parameters.RoutedPort = UART3_AT_PD9_PD8_RX_TX;
	parameters.BaudRate = UART_Baud_115200;
	parameters.ReceptionTimeOut_ms = 50;
	UART_Init( 0, parameters );


while(1)
{


  START_EXECUTION_TIME_MEASUREMENT();

  for(uint16_t k = 0 ; k <1;k++)
  {
    for(uint32_t i = 0 ; i <t;i++)
    {
    }
  }

  ex_t = GET_EXEC_TIME_US();

}



	for(;;)
	{
		switch (UART_APP_State_Machine) {
		case State_Receiving:

			UARTReturn = UART_ReceiveData(0, RXData, 300);

			if(UARTReturn == ANSWERED_REQUEST)
			{
				// Retrieve the size of received data
				RXSize = (RXData[1] << 8) |(RXData[0] << 0);
				UART_APP_State_Machine = State_Sending;
			}
			break;

		case State_Sending:
			UARTReturn = UART_SendData(0, TXData, 18);

			if(UARTReturn == ANSWERED_REQUEST)
				UART_APP_State_Machine = State_Receiving;
			break;
		}
	}
}
