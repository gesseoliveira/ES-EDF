/* *****************************************************************************
 FILE_NAME:     SPI.h
 DESCRIPTION:   SPI Driver, header file
 DESIGNER:      Plinio Barbosa da Silva
 CREATION_DATE: 28/06/2019
 VERSION:       2019.7.5
 ***************************************************************************** */

#ifndef SPI_H_INCLUDED
#define SPI_H_INCLUDED

/* *****************************************************************************
 *
 *        INCLUDES (and DEFINES for INCLUDES)
 *
 ***************************************************************************** */
#include <string.h>
#include "stdint.h"
#include "returncode.h"
#include "SETUP.h"


/* *****************************************************************************
 *        FIRMWARE VERSION
 ***************************************************************************** */
#define DRV_SPI_VER_MAJOR   2019
#define DRV_SPI_VER_MINOR   7
#define DRV_SPI_VER_PATCH   3
#define SPI_BRANCH_MASTER

/* *****************************************************************************
 *        PUBLIC DEFINITIONS
 **************************************************************************** */
#define SPI_CS_GPIO_MODE_OUTPUT 0x01

#ifndef SPI_MAX_ID_LIMIT
#define  SPI_MAX_ID_LIMIT                                                   3
#endif

/* *****************************************************************************
 *        CONFIGURATION DEFINITIONS
 **************************************************************************** */
typedef enum
{
	SPI1_AT_PA5_PA6_PA7_CLK_MISO_MOSI   = 0, /* MOSI -> PA7,  MISO -> PA6,  CLK  -> PA5  */
	SPI2_AT_PB10_PC2_PC3_CLK_MISO_MOSI     , /* MOSI -> PC3,  MISO -> PC2,  CLK  -> PB10  */
	SPI3_AT_PC10_PC11_PC12_CLK_MISO_MOSI   , /* MOSI -> PC12,  MISO -> PC11,  CLK  -> PC10  */
	SPI3_AT_PC10_PC11_PB5_CLK_MISO_MOSI      /* MOSI -> PB5,  MISO -> PC11,  CLK  -> PC10  */
} SPI_Port_Opt;

typedef enum
{
	SPI_CS_GPIO_PORT_A = 0,
	SPI_CS_GPIO_PORT_B    ,
	SPI_CS_GPIO_PORT_C   ,
	SPI_CS_GPIO_PORT_D   ,
	SPI_CS_GPIO_PORT_E   ,
	SPI_CS_GPIO_PORT_F   ,
	SPI_CS_GPIO_PORT_G   ,
	SPI_CS_GPIO_PORT_H
} SPI_CS_Port_Opt;

typedef enum
{
	POL_ACTIVE_LOW  = ((uint32_t)0x00000000U),
	POL_ACTIVE_HIGH = (0x1UL << (1U))
} SPI_Pol_Opt;

typedef enum
{
	PHA_LEADING_EDGE   = ((uint32_t)0x00000000U),
	PHA_TRAILING_EDGE  = (0x1UL << (0U))
} SPI_Pha_Opt;

typedef enum
{
	SPI_BAUD_PRESCALLER_2  = 0,
	SPI_BAUD_PRESCALLER_4     ,
	SPI_BAUD_PRESCALLER_8     ,
	SPI_BAUD_PRESCALLER_16    ,
	SPI_BAUD_PRESCALLER_32    ,
	SPI_BAUD_PRESCALLER_64    ,
	SPI_BAUD_PRESCALLER_128   ,
	SPI_BAUD_PRESCALLER_256
} SPI_BaudRate_Opt;

typedef struct
{
	  SPI_BaudRate_Opt    BaudRate;
	  SPI_Port_Opt        SelectedPort;
	  SPI_Pol_Opt         Clock_Polarity;
	  SPI_Pha_Opt         Clock_Phase;
	  uint16_t            ChipSelectPin;
	  SPI_Pol_Opt         ChipSelectPolarity;
	  uint8_t             MosiValueOnReads;    // Which value must be sent on MOSI during read operations
	  SPI_CS_Port_Opt     ChipSelectPort;
} SPI_Parameters;

typedef enum
{
	SPI_STATE_IDLE_TX_RX,
	SPI_STATE_BUSY_TX_RX,
	SPI_STATE_END_TX_RX,
} SPI_state_t;

typedef struct
{
	uint8_t 		*Rx_Buffer;
	uint16_t 		RX_Size;
	uint8_t 		*Tx_Buffer;
	uint16_t 		TX_Size;
	uint8_t 		Tranmission_Finished;
	uint8_t 		Reception_Finished;
	uint8_t			Lock;
	SPI_state_t		State;
	uint8_t			Dummy_Byte;
}SPI_Buffer_Handler_t;

extern SPI_Buffer_Handler_t SPI1_Handler, SPI2_Handler, SPI3_Handler;
/* *****************************************************************************
 *        PUBLIC PROTOTYPES
 **************************************************************************** */
/* -----------------------------------------------------------------------------
SPI_Init() - SPI configuration routine
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
ReturnCode_t SPI_Init(uint8_t ID, SPI_Parameters ConfigSPI);

/* -----------------------------------------------------------------------------
SPI_SendData() - SPI send data command
--------------------------------------------------------------------------------
Input:  ID      	- identification reference
        SendBuffer 	- source memory address pointer
        SendLength  - byte length to write
Output: void
Return: result of command
--------------------------------------------------------------------------------
Note:
----------------------------------------------------------------------------- */
ReturnCode_t SPI_SendData(uint8_t ID, uint8_t *SendBuffer, uint16_t SendLength);

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
ReturnCode_t SPI_ReceiveData(uint8_t ID, uint8_t *RecBuffer, uint16_t RecLength);

/* -----------------------------------------------------------------------------
SPI_TransmitReceiveData() - SPI Full duplex send/receive
--------------------------------------------------------------------------------
Input:  ID        	- identification reference
        SendBuffer 	- destiny memory address pointer
        SendLength 	- bytes read/write
Output: RecBuffer 	-
Return: result of command
--------------------------------------------------------------------------------
Note: This is a simple read operation where first a command is sent and after
        it the data is read. So, the provided reception buffer starts after the
        command's transmission.
      So, full byte operation count is (CmdLength + RecLength).
----------------------------------------------------------------------------- */
ReturnCode_t SPI_TransmitReceiveData(uint8_t ID, uint8_t *SendBuffer, uint8_t *RecBuffer, uint16_t SendLength);

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
ReturnCode_t SPI_DeInit(uint8_t ID);

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
ReturnCode_t SPI_Reconfigure(uint8_t ID, SPI_Parameters ConfigSPI);

#endif
