/**
 ******************************************************************************
 * @file    main.c
 * @author  Icaro Nascimento Queiroz
 * @version V1.0
 * @brief   Default main function.
 ******************************************************************************
 */


#include "sys_cfg_stm32f407.h"
#include "stm32f4xx.h"
#include "QUAD.h"
#include "returncode.h"

QUAD_Parameters_t Quad_Test;
uint32_t Counter;
float Frequency;
ReturnCode_t Result;
int8_t 		Dir;
uint32_t 	Seconds,
			Last_Seconds;

int main(void)
{
	// Initialize peripheral and power interface clock
	Sys_Enable_Peripherals_Clock();
	// Set the CPU clock and bus clock to 80MHz
	Sys_Configure_Clock_168MHz();
	Quad_Test.QUAD_Routed = TIM3_ENCODER_CH1_CH2_AT_PB4_PB5;
	QUAD_Init(0, &Quad_Test);
	for(;;)
	{
		Seconds = Sys_Get_Tick();
		//if(Seconds - Last_Seconds >= 100)
		{
			Last_Seconds = Sys_Get_Tick();
			QUAD_ReadPulses(0, &Counter);
			QUAD_Direction(0, &Dir);
		}
			QUAD_ReadFrequency(0, &Frequency);
	}
}
