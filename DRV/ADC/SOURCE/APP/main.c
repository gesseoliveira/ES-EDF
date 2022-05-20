/**
 ******************************************************************************
 * @file    main.c
 * @author  Plinio Barbosa da Silva
 * @version V1.0
 * @brief   Default main function.
 ******************************************************************************
 */


#include "stm32f4xx.h"
#include "sys_cfg_stm32f407.h"
#include "ADC.h"
#include "returncode.h"

#define N_Samples 100
#define ADC_Current_Sensor_1 0
#define ADC_Current_Sensor_2 1
#define ADC_Current_Sensor_3 2

ADC_Parameters_t      ADC_Setup;
ADC_Channel_Config_t  ADC_Channel_Configuration;
uint32_t ADC_Buffer1[N_Samples];
uint32_t ADC_Buffer2[N_Samples];
uint32_t ADC_Buffer3[N_Samples];

int main(void)
{
	int state1 = 0;
	int state2 = 0;
	int state3 = 0;
	ReturnCode_t return1, return2, return3;

	// Initialize peripheral and power interface clock
	Sys_Enable_Peripherals_Clock();
	// Set the CPU clock and bus clock to 80MHz
	Sys_Configure_Clock_168MHz();

	ADC_Setup.Clock_Prescaler 	= ADC_CLOCK_DIV8;
	ADC_Setup.Output_Resolution = ADC_RES_12_BITS;

	ADC_Setup.ADC_Hardware = ADC_1;
	ADC_Init(0, ADC_Setup);

	ADC_Setup.ADC_Hardware = ADC_2;
	ADC_Init(1, ADC_Setup);

	ADC_Setup.ADC_Hardware = ADC_3;
	ADC_Init(2, ADC_Setup);

	ADC_Channel_Configuration.Channel = ADC123_CH_0_AT_PA0;
	ADC_Channel_Configuration.Channel_Sampling_Time = ADC_SAMPLE_TIME_480CYCLES;
	ADC_Setup_Channel(ADC_Current_Sensor_1, 0, ADC_Channel_Configuration);

	ADC_Channel_Configuration.Channel = ADC123_CH_1_AT_PA1;
	ADC_Channel_Configuration.Channel_Sampling_Time = ADC_SAMPLE_TIME_480CYCLES;
	ADC_Setup_Channel(ADC_Current_Sensor_2, 1, ADC_Channel_Configuration);

	ADC_Channel_Configuration.Channel = ADC123_CH_2_AT_PA2;
	ADC_Channel_Configuration.Channel_Sampling_Time = ADC_SAMPLE_TIME_480CYCLES;
	ADC_Setup_Channel(ADC_Current_Sensor_3, 2, ADC_Channel_Configuration);

	while (1)
	{
		if (state1 == 0)
		{
			return1 = ADC_Read(ADC_Current_Sensor_1, ADC_Buffer1, N_Samples);
			if(return1 == ANSWERED_REQUEST)
			{
				state1 = 1;
			}
		}
		if (state2 == 0)
		{
			return2 = ADC_Read(ADC_Current_Sensor_2, ADC_Buffer2, N_Samples);
			if(return2 == ANSWERED_REQUEST)
			{
				state2 = 1;
			}
		}
		if (state3 == 0)
		{
			return3 = ADC_Read(ADC_Current_Sensor_3, ADC_Buffer3, N_Samples);
			if(return3 == ANSWERED_REQUEST)
			{
				state3 = 1;
			}
		}
		if (return1 + return2 + return3 == ANSWERED_REQUEST)
		{
			state1 = 0;
			state2 = 0;
			state3 = 0;
		}
	}
	return 0;
}
