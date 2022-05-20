/* *****************************************************************************
 FILE_NAME:     SPI.c
 DESCRIPTION:   SPI Driver, Source file
 DESIGNER:      Plinio Barbosa da Silva
 CREATION_DATE: 28/06/2019
 VERSION:       2019.7.5
 **************************************************************************** */

#include "SPI.h"
#include "stm32f4xx.h"
#include <string.h>
#include "returncode.h"

/* *****************************************************************************
 *        PRIVATE DEFINITIONS
 **************************************************************************** */
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

/* The variable below is declared by the clock manager; it holds the current  */
/*  core clock.                                                               */
//extern uint32_t SystemCoreClock;
/* *****************************************************************************
 *        PRIVATE VARIABLES
 **************************************************************************** */
static SPI_Parameters SPI_DeviceList[SPI_MAX_ID_LIMIT];
SPI_Buffer_Handler_t SPI1_Handler, SPI2_Handler, SPI3_Handler;

typedef struct
{
	SPI_TypeDef 			*SPI_Instance;
	GPIO_TypeDef			*CS_GPIO_Port;
	uint32_t				CS_GPIO_Pin;
	SPI_Buffer_Handler_t	*Buffer_Status;
	SPI_Parameters			*SPI_Device_Parameters;
	uint8_t					Initilization_Status;
}SPI_Handler_t;

SPI_Handler_t	SPI_Device_Ctrl[SPI_MAX_ID_LIMIT];

/* *****************************************************************************
 *        PRIVATE PROTOTYPES
 **************************************************************************** */
static void   	SPI_ChipSelect(uint8_t ID);
static void   	SPI_CSinit(uint8_t ID);
static void   	SPI_ChipUnselect(uint8_t ID);

static void   	SPI_Device_Setup(SPI_Handler_t	SPI_Device);
ReturnCode_t   	SPI_Device_Send(uint8_t ID, uint8_t *SendBuffer, uint16_t SendLength);
ReturnCode_t   	SPI_Device_Receive(uint8_t ID, uint8_t *RecvBuffer, uint16_t RecvLenght);
ReturnCode_t  	SPI_Device_Send_Receive(uint8_t ID, uint8_t *SendBuffer, uint8_t *RecvBuffer, uint16_t Size);
//static uint8_t	SPI_Lock(spi_handler_t SPI_Handler);
//static void 	SPI_Unlock(spi_handler_t SPI_Handler);

/* *****************************************************************************
 *        PUBLIC FUNCTIONS / ROUTINES
 **************************************************************************** */
/* -----------------------------------------------------------------------------
SPI_Config() - SPI configuration routine
--------------------------------------------------------------------------------
Input:  ID - ID that should be allocated and configured.
        Config - Pointer to the desired SPI's parameters
Output: None
Return: ANSWERED_REQUEST - All ok, config has been successful.
        Else             - Some error happened.
--------------------------------------------------------------------------------
Note: This routine will allocate an ID an will make it able to use a SPI
        as passed through the configuration.
----------------------------------------------------------------------------- */
ReturnCode_t SPI_Init(uint8_t ID, SPI_Parameters ConfigSPI)
{
	ReturnCode_t  ReturnCode;

	memcpy(&SPI_DeviceList[ID], &ConfigSPI, sizeof(SPI_Parameters));

	switch (SPI_DeviceList[ID].SelectedPort)
	{
	case SPI1_AT_PA5_PA6_PA7_CLK_MISO_MOSI:
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); // Enable GPIOA Clock
		/* GPIOA Pin 5 As SPI1 SCK */
		CLR_GPIO_AFRL(GPIOA , 5);
		SET_GPIO_AFRL(GPIOA , 5, 5);
		CLR_GPIO_MODE(GPIOA , 5);
		CLR_GPIO_PULL(GPIOA , 5);
		CLR_GPIO_SPEED(GPIOA, 5);
		SET_GPIO_MODE(GPIOA , 5, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOA , 5, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOA, 5, GPIO_OSPEED_HIGH_SPEED_100MHZ);
		/* GPIOA Pin 6 As SPI1 MISO */
		CLR_GPIO_AFRL(GPIOA , 6);
		SET_GPIO_AFRL(GPIOA , 6, 5);
		CLR_GPIO_MODE(GPIOA , 6);
		CLR_GPIO_PULL(GPIOA , 6);
		CLR_GPIO_SPEED(GPIOA, 6);
		SET_GPIO_MODE(GPIOA , 6, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOA , 6, GPIO_PUPDR_PULL_UP);
		/* GPIOA Pin 7 As SPI1 MOSI */
		CLR_GPIO_AFRL(GPIOA , 7);
		SET_GPIO_AFRL(GPIOA , 7, 5);
		CLR_GPIO_MODE(GPIOA , 7);
		CLR_GPIO_PULL(GPIOA , 7);
		CLR_GPIO_SPEED(GPIOA, 7);
		SET_GPIO_MODE(GPIOA , 7, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOA , 7, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOA, 7, GPIO_OSPEED_HIGH_SPEED_100MHZ);

		SPI_Device_Ctrl[ID].SPI_Instance 				= SPI1;
		SPI_Device_Ctrl[ID].SPI_Device_Parameters		= &SPI_DeviceList[ID];
		SPI_Device_Ctrl[ID].Buffer_Status				= &SPI1_Handler;
		SPI_Device_Ctrl[ID].Buffer_Status->Dummy_Byte	= SPI_Device_Ctrl[ID].SPI_Device_Parameters->MosiValueOnReads;
		SPI_Device_Ctrl[ID].Buffer_Status->State		=	SPI_STATE_IDLE_TX_RX;

		SPI_Device_Setup(SPI_Device_Ctrl[ID]);

		ReturnCode = ANSWERED_REQUEST;
		break;
	case SPI2_AT_PB10_PC2_PC3_CLK_MISO_MOSI:
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN); // Enable GPIOB Clock
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // Enable GPIOC Clock

		/* GPIOB Pin 10 As SPI1 SCK */
		CLR_GPIO_AFRH(GPIOB , 10);
		SET_GPIO_AFRH(GPIOB , 10, 5);
		CLR_GPIO_MODE(GPIOB , 10);
		CLR_GPIO_PULL(GPIOB , 10);
		CLR_GPIO_SPEED(GPIOB, 10);
		SET_GPIO_MODE(GPIOB , 10, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOB , 10, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOB, 10, GPIO_OSPEED_HIGH_SPEED_100MHZ);
		/* GPIOC Pin 2 As SPI1 MISO */
		CLR_GPIO_AFRL(GPIOC , 2);
		SET_GPIO_AFRL(GPIOC , 2, 5);
		CLR_GPIO_MODE(GPIOC , 2);
		CLR_GPIO_PULL(GPIOC , 2);
		CLR_GPIO_SPEED(GPIOC, 2);
		SET_GPIO_MODE(GPIOC , 2, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 2, GPIO_PUPDR_PULL_UP);
		/* GPIOC Pin 3 As SPI1 MOSI */
		CLR_GPIO_AFRL(GPIOC , 3);
		SET_GPIO_AFRL(GPIOC , 3, 5);
		CLR_GPIO_MODE(GPIOC , 3);
		CLR_GPIO_PULL(GPIOC , 3);
		CLR_GPIO_SPEED(GPIOC, 3);
		SET_GPIO_MODE(GPIOC , 3, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 3, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOC, 3, GPIO_OSPEED_HIGH_SPEED_100MHZ);

		SPI_Device_Ctrl[ID].SPI_Instance 			= SPI2;
		SPI_Device_Ctrl[ID].SPI_Device_Parameters	= &SPI_DeviceList[ID];
		SPI_Device_Ctrl[ID].Buffer_Status			= &SPI2_Handler;
		SPI_Device_Ctrl[ID].Buffer_Status->Dummy_Byte	= SPI_Device_Ctrl[ID].SPI_Device_Parameters->MosiValueOnReads;
		SPI_Device_Ctrl[ID].Buffer_Status->State	=	SPI_STATE_IDLE_TX_RX;

		SPI_Device_Setup(SPI_Device_Ctrl[ID]);

		ReturnCode = ANSWERED_REQUEST;
		break;
	case SPI3_AT_PC10_PC11_PC12_CLK_MISO_MOSI:
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // Enable GPIOC Clock

		/* GPIOB Pin 10 As SPI1 SCK */
		CLR_GPIO_AFRH(GPIOC , 10);
		SET_GPIO_AFRH(GPIOC , 10, 6);
		CLR_GPIO_MODE(GPIOC , 10);
		CLR_GPIO_PULL(GPIOC , 10);
		CLR_GPIO_SPEED(GPIOC, 10);
		SET_GPIO_MODE(GPIOC , 10, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 10, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOC, 10, GPIO_OSPEED_HIGH_SPEED_100MHZ);
		/* GPIOC Pin 2 As SPI1 MISO */
		CLR_GPIO_AFRH(GPIOC , 11);
		SET_GPIO_AFRH(GPIOC , 11, 6);
		CLR_GPIO_MODE(GPIOC , 11);
		CLR_GPIO_PULL(GPIOC , 11);
		CLR_GPIO_SPEED(GPIOC, 11);
		SET_GPIO_MODE(GPIOC , 11, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 11, GPIO_PUPDR_PULL_UP);
		/* GPIOC Pin 3 As SPI1 MOSI */
		CLR_GPIO_AFRH(GPIOC , 12);
		SET_GPIO_AFRH(GPIOC , 12, 6);
		CLR_GPIO_MODE(GPIOC , 12);
		CLR_GPIO_PULL(GPIOC , 12);
		CLR_GPIO_SPEED(GPIOC, 12);
		SET_GPIO_MODE(GPIOC , 12, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 12, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOC, 12, GPIO_OSPEED_HIGH_SPEED_100MHZ);

		SPI_Device_Ctrl[ID].SPI_Instance 			= SPI3;
		SPI_Device_Ctrl[ID].SPI_Device_Parameters	= &SPI_DeviceList[ID];
		SPI_Device_Ctrl[ID].Buffer_Status			= &SPI3_Handler;
		SPI_Device_Ctrl[ID].Buffer_Status->Dummy_Byte	= SPI_Device_Ctrl[ID].SPI_Device_Parameters->MosiValueOnReads;
		SPI_Device_Ctrl[ID].Buffer_Status->State	=	SPI_STATE_IDLE_TX_RX;

		SPI_Device_Setup(SPI_Device_Ctrl[ID]);

		ReturnCode = ANSWERED_REQUEST;
		break;
	case SPI3_AT_PC10_PC11_PB5_CLK_MISO_MOSI:
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN); // Enable GPIOB Clock
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // Enable GPIOC Clock

		/* GPIOC Pin 10 As SPI1 SCK */
		CLR_GPIO_AFRH(GPIOC , 10);
		SET_GPIO_AFRH(GPIOC , 10, 6);
		CLR_GPIO_MODE(GPIOC , 10);
		CLR_GPIO_PULL(GPIOC , 10);
		CLR_GPIO_SPEED(GPIOC, 10);
		SET_GPIO_MODE(GPIOC , 10, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 10, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOC, 10, GPIO_OSPEED_HIGH_SPEED_100MHZ);
		/* GPIOC Pin 11 As SPI1 MISO */
		CLR_GPIO_AFRH(GPIOC , 11);
		SET_GPIO_AFRH(GPIOC , 11, 6);
		CLR_GPIO_MODE(GPIOC , 11);
		CLR_GPIO_PULL(GPIOC , 11);
		CLR_GPIO_SPEED(GPIOC, 11);
		SET_GPIO_MODE(GPIOC , 11, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOC , 11, GPIO_PUPDR_PULL_UP);
		/* GPIOB Pin 5 As SPI1 MOSI */
		CLR_GPIO_AFRL(GPIOB , 5);
		SET_GPIO_AFRL(GPIOB , 5, 6);
		CLR_GPIO_MODE(GPIOB , 5);
		CLR_GPIO_PULL(GPIOB , 5);
		CLR_GPIO_SPEED(GPIOB, 5);
		SET_GPIO_MODE(GPIOB , 5, GPIO_MODER_MODE_ALT_FUNC);
		SET_GPIO_PULL(GPIOB , 5, GPIO_PUPDR_NO_PULL);
		SET_GPIO_SPEED(GPIOB, 5, GPIO_OSPEED_HIGH_SPEED_100MHZ);

		SPI_Device_Ctrl[ID].SPI_Instance 			= SPI3;
		SPI_Device_Ctrl[ID].SPI_Device_Parameters	= &SPI_DeviceList[ID];
		SPI_Device_Ctrl[ID].Buffer_Status			= &SPI3_Handler;
		SPI_Device_Ctrl[ID].Buffer_Status->Dummy_Byte	= SPI_Device_Ctrl[ID].SPI_Device_Parameters->MosiValueOnReads;
		SPI_Device_Ctrl[ID].Buffer_Status->State	=	SPI_STATE_IDLE_TX_RX;

		SPI_Device_Setup(SPI_Device_Ctrl[ID]);

		ReturnCode = ANSWERED_REQUEST;
		break;
	default:
		ReturnCode = ERR_PARAM_VALUE;
		break;
	}

	/* -------------------- Init the CS pin ---- ---------------------------- */
	SPI_CSinit(ID);

	SPI_ChipUnselect(ID);

	return ReturnCode;
}

/* -----------------------------------------------------------------------------
SPI_ReceiveData() - SPI Read command
--------------------------------------------------------------------------------
Input:  ID        - identification reference
        CmdBuffer - destiny memory address pointer
        CmdLength - source device address
        RecLength - byte length to read
Output: RecBuffer -
Return: result of command
--------------------------------------------------------------------------------
Note: This is a simple read operation where first a command is sent and after
        it the data is read. So, the provided reception buffer starts after the
        command's transmission.
      So, full byte operation count is (CmdLength + RecLength).
----------------------------------------------------------------------------- */
ReturnCode_t SPI_TransmitReceiveData(uint8_t ID, uint8_t *SendBuffer, uint8_t *RecBuffer, uint16_t SendLength)
{
	ReturnCode_t ReturnCode;

	//	if ( SPI_IsIdInit[ID] == FALSE)
	//	{
	//		ReturnCode = ERR_DISABLED;
	//	}
	if (ID >= SPI_MAX_ID_LIMIT)
	{
		ReturnCode = ERR_PARAM_ID;
	}
	else
	{
		ReturnCode = SPI_Device_Send_Receive(ID, SendBuffer, RecBuffer, SendLength);
	}

	return ReturnCode;
}

/* -----------------------------------------------------------------------------
SPI_SendData() - SPI send data command
--------------------------------------------------------------------------------
Input:  ID      - identification reference
        SendBuffer - source memory address pointer
        Length  - byte length to write
Output: void
Return: result of command
--------------------------------------------------------------------------------
Note:
----------------------------------------------------------------------------- */
ReturnCode_t SPI_SendData(uint8_t ID, uint8_t *SendBuffer, uint16_t SendLength)
{
	ReturnCode_t ReturnCode;

	//	if ( SPI_IsIdInit[ID] == FALSE)
	//	{
	//		ReturnCode = ERR_DISABLED;
	//	}
	if (ID >= SPI_MAX_ID_LIMIT)
	{
		ReturnCode = ERR_PARAM_ID;
	}
	else
	{
		ReturnCode = SPI_Device_Send(ID, SendBuffer, SendLength);
	}

	return ReturnCode;
}

/* -----------------------------------------------------------------------------
SPI_ReceiveData() - SPI receive data command
--------------------------------------------------------------------------------
Input:  ID      - identification reference
        SendBuffer - source memory address pointer
        SendLength  - byte length to write
Output: void
Return: result of command
--------------------------------------------------------------------------------
Note:
----------------------------------------------------------------------------- */
ReturnCode_t SPI_ReceiveData(uint8_t ID, uint8_t *RecBuffer, uint16_t RecLength)
{
	ReturnCode_t ReturnCode;

	//	if ( SPI_IsIdInit[ID] == FALSE)
	//	{
	//		ReturnCode = ERR_DISABLED;
	//	}
	if (ID >= SPI_MAX_ID_LIMIT)
	{
		ReturnCode = ERR_PARAM_ID;
	}
	else
	{
		ReturnCode = SPI_Device_Receive(ID, RecBuffer, RecLength);
	}

	return ReturnCode;
}
/* -----------------------------------------------------------------------------
SPI_DeInit() - SPI De-Initialization routine, returns the driver to reset state
--------------------------------------------------------------------------------
Input:  ID - ID that should be allocated and configured.
Output: None
Return: ANSWERED_REQUEST - All ok, de-initilation has been successful.
        Else             - Some error happened.
--------------------------------------------------------------------------------
Note: This routine will allocate an ID an will make it able to use a SPI
        as passed through the configuration.
----------------------------------------------------------------------------- */
ReturnCode_t SPI_DeInit(uint8_t ID)
{
	ReturnCode_t RetCode;

	RetCode = ANSWERED_REQUEST;
	return RetCode;
}

/* -----------------------------------------------------------------------------
SPI_Reconfigure() - SPI reconfiguration routine
--------------------------------------------------------------------------------
Input:  ID - ID that should be allocated and configured.
	    Config - Pointer to the new desired SPI's parameters

Output: None
Return: ANSWERED_REQUEST - All ok, de-initilation has been successful.
        Else             - Some error happened.
--------------------------------------------------------------------------------
Note: This routine will allocate an ID an will make it able to use a SPI
        as passed through the configuration.
----------------------------------------------------------------------------- */
ReturnCode_t SPI_Reconfigure(uint8_t ID, SPI_Parameters ConfigSPI)
{
	ReturnCode_t RetCode;

	RetCode = ANSWERED_REQUEST;
	return RetCode;
}

/* *****************************************************************************
 *        PRIVATE FUNCTIONS / ROUTINES
 **************************************************************************** */
/* -----------------------------------------------------------------------------
SPI_CSinit() - Initialize the Chip Select Pin (or NSS)
--------------------------------------------------------------------------------
Input:  ID - ID that will initialize the NSS pin
Output: None
Return: None
--------------------------------------------------------------------------------
Note: This routine initialize the Chip Select Pin (or NSS), according to the
adjustments made in the structure indicated by ID.
----------------------------------------------------------------------------- */
static void SPI_CSinit(uint8_t ID)
{
	if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_A)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOA;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_B)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOB;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_C)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOC;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_D)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOD;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_E)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOE;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_F)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOF;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_G)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOG;
	}
	else if(SPI_DeviceList[ID].ChipSelectPort == SPI_CS_GPIO_PORT_H)
	{
		SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN);
		SPI_Device_Ctrl[ID].CS_GPIO_Port	=	GPIOH;
	}

	// Sets Output Mode to: General purpose output mode
	SET_BIT(  SPI_Device_Ctrl[ID].CS_GPIO_Port->MODER,  1 << ( 2 * SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin));
	// Sets Output Mode to: General purpose output mode
	CLEAR_BIT(SPI_Device_Ctrl[ID].CS_GPIO_Port->MODER,  1 << ((2 * SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin) + 1));
	// Sets Output Type to: Output push-pull
	CLEAR_BIT(SPI_Device_Ctrl[ID].CS_GPIO_Port->OTYPER, 1 << SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin);
	SPI_Device_Ctrl[ID].CS_GPIO_Port->OSPEEDR |= (0x3 << (SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin * 2));


}

/* -----------------------------------------------------------------------------
SPI_ChipSelect() - Activates the Chip Select Pin (or NSS)
--------------------------------------------------------------------------------
Input:  ID - ID of the device to be activated
Output: None
Return: None
--------------------------------------------------------------------------------
Note: This routine activate the Chip Select Pin (or NSS), according to the
polarity specified by ID.
----------------------------------------------------------------------------- */
static void SPI_ChipSelect(uint8_t ID)
{
	if(SPI_DeviceList[ID].ChipSelectPolarity == POL_ACTIVE_LOW)
		CLEAR_BIT(SPI_Device_Ctrl[ID].CS_GPIO_Port->ODR, 1 << SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin);
	else
		SET_BIT(SPI_Device_Ctrl[ID].CS_GPIO_Port->ODR, 1 << SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin);
}

/* -----------------------------------------------------------------------------
SPI_ChipUnselect() - Deactivates the Chip Select Pin (or NSS)
--------------------------------------------------------------------------------
Input:  ID - ID of the device to be activated
Output: None
Return: None
--------------------------------------------------------------------------------
Note: This routine deactivates the Chip Select Pin (or NSS), according to the
polarity specified by ID.
----------------------------------------------------------------------------- */
static void SPI_ChipUnselect(uint8_t ID)
{
	if(SPI_DeviceList[ID].ChipSelectPolarity == POL_ACTIVE_LOW)
		SET_BIT(SPI_Device_Ctrl[ID].CS_GPIO_Port->ODR, 1 << SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin);
	else
		CLEAR_BIT(SPI_Device_Ctrl[ID].CS_GPIO_Port->ODR, 1 << SPI_Device_Ctrl[ID].SPI_Device_Parameters->ChipSelectPin);
}

ReturnCode_t   	SPI_Device_Send(uint8_t ID, uint8_t *SendBuffer, uint16_t SendLength)
{
	ReturnCode_t ReturnCode;
	uint32_t Dummy_Read;

	ReturnCode = OPERATION_RUNNING;

	switch (SPI_Device_Ctrl[ID].Buffer_Status->State)
	{
	case SPI_STATE_IDLE_TX_RX:
		SPI_ChipSelect(ID);
		SPI_Device_Ctrl[ID].Buffer_Status->RX_Size   = SendLength;
		SPI_Device_Ctrl[ID].Buffer_Status->Rx_Buffer = NULL;
		SPI_Device_Ctrl[ID].Buffer_Status->TX_Size   = SendLength;
		SPI_Device_Ctrl[ID].Buffer_Status->Tx_Buffer = SendBuffer;
		SPI_Device_Ctrl[ID].Buffer_Status->Reception_Finished = 0;
		SPI_Device_Ctrl[ID].Buffer_Status->Tranmission_Finished = 0;

		Dummy_Read = SPI_Device_Ctrl[ID].SPI_Instance->SR;
		Dummy_Read = SPI_Device_Ctrl[ID].SPI_Instance->DR;
		(void)Dummy_Read;
		//SPI_Device_Ctrl[ID].SPI_Instance->DR = SPI_Device_Ctrl[ID].Buffer_Status->Dummy_Byte;
		SET_BIT(SPI_Device_Ctrl[ID].SPI_Instance->CR2, SPI_CR2_RXNEIE);	// Enable RX not empty interrupt [Data available on DR register]
		SET_BIT(SPI_Device_Ctrl[ID].SPI_Instance->CR2, SPI_CR2_TXEIE);	// TX empty interrupt [SPI ready to transmit data]
		SPI_Device_Ctrl[ID].Buffer_Status->State = SPI_STATE_BUSY_TX_RX;
		break;
	case SPI_STATE_BUSY_TX_RX:
		break;
	case SPI_STATE_END_TX_RX:
		SPI_ChipUnselect(ID);
		ReturnCode = ANSWERED_REQUEST;
		SPI_Device_Ctrl[ID].Buffer_Status->State = SPI_STATE_IDLE_TX_RX;
		break;
	}

	return ReturnCode;
}

ReturnCode_t   	SPI_Device_Receive(uint8_t ID, uint8_t *RecvBuffer, uint16_t RecvLenght)
{
	ReturnCode_t ReturnCode;
	uint32_t Dummy_Read;

	ReturnCode = OPERATION_RUNNING;

	switch (SPI_Device_Ctrl[ID].Buffer_Status->State)
	{
	case SPI_STATE_IDLE_TX_RX:
		SPI_ChipSelect(ID);
		SPI_Device_Ctrl[ID].Buffer_Status->RX_Size   = RecvLenght;
		SPI_Device_Ctrl[ID].Buffer_Status->Rx_Buffer = RecvBuffer;
		SPI_Device_Ctrl[ID].Buffer_Status->TX_Size   = RecvLenght;
		SPI_Device_Ctrl[ID].Buffer_Status->Tx_Buffer = NULL;
		SPI_Device_Ctrl[ID].Buffer_Status->Reception_Finished = 0;
		SPI_Device_Ctrl[ID].Buffer_Status->Tranmission_Finished = 0;

		Dummy_Read = SPI1->SR;
		Dummy_Read = SPI1->DR;
		(void)Dummy_Read;

		SET_BIT(SPI_Device_Ctrl[ID].SPI_Instance->CR2, SPI_CR2_RXNEIE);	// Enable RX not empty interrupt [Data available on DR register]
		SET_BIT(SPI_Device_Ctrl[ID].SPI_Instance->CR2, SPI_CR2_TXEIE);	// TX empty interrupt [SPI ready to transmit data]
		SPI_Device_Ctrl[ID].Buffer_Status->State = SPI_STATE_BUSY_TX_RX;
		break;
	case SPI_STATE_BUSY_TX_RX:
		break;
	case SPI_STATE_END_TX_RX:
		SPI_ChipUnselect(ID);
		ReturnCode = ANSWERED_REQUEST;
		SPI_Device_Ctrl[ID].Buffer_Status->State = SPI_STATE_IDLE_TX_RX;
		break;
	}

	return ReturnCode;
}

ReturnCode_t  	SPI_Device_Send_Receive(uint8_t ID, uint8_t *SendBuffer, uint8_t *RecvBuffer, uint16_t Size)
{
	ReturnCode_t ReturnCode;
	uint32_t Dummy_Read;

	ReturnCode = OPERATION_RUNNING;

	switch (SPI_Device_Ctrl[ID].Buffer_Status->State)
	{
	case SPI_STATE_IDLE_TX_RX:
		SPI_ChipSelect(ID);
		SPI_Device_Ctrl[ID].Buffer_Status->RX_Size   = Size;
		SPI_Device_Ctrl[ID].Buffer_Status->Rx_Buffer = RecvBuffer;
		SPI_Device_Ctrl[ID].Buffer_Status->TX_Size   = Size;
		SPI_Device_Ctrl[ID].Buffer_Status->Tx_Buffer = SendBuffer;
		SPI_Device_Ctrl[ID].Buffer_Status->Reception_Finished = 0;
		SPI_Device_Ctrl[ID].Buffer_Status->Tranmission_Finished = 0;

		Dummy_Read = SPI1->SR;
		Dummy_Read = SPI1->DR;
		(void)Dummy_Read;

		SET_BIT(SPI_Device_Ctrl[ID].SPI_Instance->CR2, SPI_CR2_RXNEIE);	// Enable RX not empty interrupt [Data available on DR register]
		SET_BIT(SPI_Device_Ctrl[ID].SPI_Instance->CR2, SPI_CR2_TXEIE);	// TX empty interrupt [SPI ready to transmit data]
		SPI_Device_Ctrl[ID].Buffer_Status->State = SPI_STATE_BUSY_TX_RX;
		break;
	case SPI_STATE_BUSY_TX_RX:
		break;
	case SPI_STATE_END_TX_RX:
		SPI_ChipUnselect(ID);
		ReturnCode = ANSWERED_REQUEST;
		SPI_Device_Ctrl[ID].Buffer_Status->State = SPI_STATE_IDLE_TX_RX;
		break;
	}

	return ReturnCode;
}

static void   	SPI_Device_Setup(SPI_Handler_t	SPI_Device)
{
	/* -------------------- Init the SPI peripheral ------------------------- */
	/*
	 * STM32F407 SPI
	 * 1 is attached to APB2 -> 84MHz
	 * 2 and 3 is attached to APB1  -> 42MHz
	 */
	if((SPI_Device.SPI_Instance) == SPI1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	}
	else if((SPI_Device.SPI_Instance) == SPI2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	}
	else if((SPI_Device.SPI_Instance) == SPI3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	}


	SPI_Device.SPI_Instance->CR1 = 0x00;

	WRITE_REG(SPI_Device.SPI_Instance->CR1,((SPI_CR1_MSTR | SPI_CR1_SSI)                      |                  // Specifies the SPI operating mode: Always Master
			SPI_Device.SPI_Device_Parameters->Clock_Polarity |                  // Sets Polarity of clock
			SPI_Device.SPI_Device_Parameters->Clock_Phase    |                  // Sets Phase data capture and send
			SPI_CR1_SSM                                                         // NSS always controlled by software
	));

	SPI_Device.SPI_Instance->CR1 |= (SPI_Device.SPI_Device_Parameters->BaudRate << SPI_CR1_BR_Pos);
	SPI_Device.SPI_Instance->CR2  = 0;

	// Enable SPI
	SPI_Device.SPI_Instance->CR1 |= SPI_CR1_SPE;


	// Enable SPI Interrupts
	if((SPI_Device.SPI_Instance) == SPI1)
	{
		NVIC_SetPriority(SPI1_IRQn, NVIC_EncodePriority(0, 0, 0));
		NVIC_EnableIRQ(SPI1_IRQn);
	}
	else if((SPI_Device.SPI_Instance) == SPI2)
	{
		NVIC_SetPriority(SPI2_IRQn, NVIC_EncodePriority(0, 0, 0));
		NVIC_EnableIRQ(SPI2_IRQn);
	}
	else if((SPI_Device.SPI_Instance) == SPI3)
	{
		NVIC_SetPriority(SPI3_IRQn, NVIC_EncodePriority(0, 0, 0));
		NVIC_EnableIRQ(SPI3_IRQn);;
	}
}



//uint8_t	SPI_Lock(spi_handler_t SPI_Handler)
//{
//	if(SPI_Handler.Lock == 1){
//		return 0;
//	}
//	else{
//		SPI_Handler.Lock = 1;
//		return 1;
//	}
//	return 0;
//}
//void 	SPI_Unlock(spi_handler_t SPI_Handler)
//{
//	SPI_Handler.Lock = 0;
//}

