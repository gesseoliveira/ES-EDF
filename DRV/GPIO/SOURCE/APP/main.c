/**
 ******************************************************************************
 * @file    main.c
 * @author  Plinio Barbosa da Silva
 * @version V1.0
 * @brief   Default main function.
 ******************************************************************************
 */


#include "stm32f4xx.h"
#include "returncode.h"
#include "GPIO.h"
#include "sys_cfg_stm32f407.h"

#define BUTTON_ID  0
#define LED1_ID    1

int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	uint8_t GPIO_Input_Value;
	GPIO_Parameters_t GPIO_Configuration;

	// Initialize peripheral and power interface clock
	Sys_Enable_Peripherals_Clock();
	// Set the CPU clock and bus clock to 80MHz
	Sys_Configure_Clock_168MHz();

	GPIO_Configuration.GPIO_Direction = GPIO_MODE_IN_PULL_UP;
	GPIO_Configuration.GPIO_Pin 	= PIN_4;
	GPIO_Configuration.GPIO_Port	= GPIO_PORT_E;
	GPIO_Init(BUTTON_ID, GPIO_Configuration);

	GPIO_Configuration.GPIO_Direction= GPIO_MODE_OUT_PP;
	GPIO_Configuration.GPIO_Pin 	= PIN_2;
	GPIO_Configuration.GPIO_Port	= GPIO_PORT_E;
	GPIO_Init(LED1_ID, GPIO_Configuration);

	for(;;)
	{
		GPIO_ReadInput(BUTTON_ID, &GPIO_Input_Value);
		if(GPIO_Input_Value == 1)
		{
			GPIO_SetOutput(LED1_ID);
		}
		else
		{
			GPIO_ClearOutput(LED1_ID);
		}
	}
}
