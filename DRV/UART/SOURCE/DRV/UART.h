/**
 ******************************************************************************
 * @file    UART.h
 * @author  Plinio Barbosa da Silva
 * @brief   UART module driver.
 * @brief   Version 2019.9.1
 * @brief   Creation Date 28/03/2018
 *
 *          This file provides functions to manage UART
 *          functionalities:
 *           + Initialization of a UART
 *           + Read data from UART
 *           + Write data to UART
 *
 *  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================

  ...

UART_Config parameters;

void SystemClock_Config(void);

uint8_t XBEE_UART = 1;
uint8_t USB_SERIAL = 2;
uint8_t TXData[] = {"Hello World!!!\n\t"};
uint8_t RXData[20];
uint16_t RXSize;
int y = 0;

int main(void)
{

  ReturnCode_t UARTReturn;
  UART_Config parameters;

  static enum { State_Receiving, State_Sending } UART_APP_State_Machine = State_Receiving;

  HAL_Init();

  SystemClock_Config();

  parameters.RoutedPort = UART1_AT_PA10_PA9_RX_TX;
  parameters.BaudRate = 115200;
  parameters.ReceptionTimeOut_ms = 5;
  UART_Init( XBEE_UART, parameters );

  while(1)
  {
    switch (UART_APP_State_Machine) {
    case State_Receiving:

      UARTReturn = UART_ReceiveData(XBEE_UART, RXData, 20);

      if(UARTReturn == ANSWERED_REQUEST)
        UART_APP_State_Machine = State_Sending;

      break;

    case State_Sending:
      UARTReturn = UART_SendData(XBEE_UART, RXData, RXData[0]);

      if(UARTReturn == ANSWERED_REQUEST)
        UART_APP_State_Machine = State_Receiving;

      break;
    }
    y++;
  }
}


  ...

  @endverbatim
 */


#ifndef UART_H_
#define UART_H_


#include "returncode.h"
#include "stm32f4xx.h"
#include "SETUP.h"

#ifndef UART_MAX_ID_LIMIT
#define UART_MAX_ID_LIMIT 5
#endif

#define UART_NOT_BUSY 255

#define UART_ABORT_RECEPTION       0xFFFFFFFF

typedef struct
{
  uint32_t Last_Receive_Timestamp;
  //uint8_t   Transmit_Buffer[UART_RX_TX_BUFFER_SIZE];
  uint8_t   *Transmit_Buffer;
  uint32_t  Transmit_Counter;
  uint32_t  Receive_Counter;
  //uint8_t   Receive_Buffer[UART_RX_TX_BUFFER_SIZE];
  uint8_t   *Receive_Buffer;
  uint8_t  Locked_ID;
}UART_Ctrl_Data_t;

extern UART_Ctrl_Data_t UART1_Control;
extern UART_Ctrl_Data_t UART2_Control;
extern UART_Ctrl_Data_t UART3_Control;
extern UART_Ctrl_Data_t UART6_Control;

/***** Config structure *****/
typedef enum
{
  UART1_AT_PA10_PA9_RX_TX,
  UART2_AT_PD6_PD5_RX_TX,
  UART3_AT_PD9_PD8_RX_TX,
  UART6_AT_PC6_PC7_RX_TX,
} RoutedPortOptions;

typedef enum
{
  UART_Baud_9600,
  UART_Baud_19200,
  UART_Baud_57600,
  UART_Baud_115200,
  UART_Baud_230400,
  UART_Baud_460800,
  UART_Baud_921600
}UART_Baudrate_t;

typedef enum
{
  UART_STATE_IDLE = 0,
  UART_STATE_TRANSMITING,
  UART_STATE_RECEIVING,
} UART_State_Control_t;
typedef struct
{
  RoutedPortOptions          RoutedPort;
  UART_Baudrate_t            BaudRate;
  uint32_t                   ReceptionTimeOut_ms;
  UART_State_Control_t       State;
  uint32_t                   Tx_Size;
  uint8_t                    *Tx_Buffer;
  uint32_t                   Rx_Size;
  uint8_t                    *Rx_Buffer;
}UART_Config_t;

/**
 * @brief  UART configuration routine.
 * @param  ID ID that should be allocated and configured.
 * @param  UART_Config to the desired UART's parameters
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t UART_Init(uint8_t ID, UART_Config_t ConfigUART);

/**
 * @brief  Basic send data routine.
 * @param  ID UART ID number.
 * @param  SendBuffer Pointer to where the data is.
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t UART_SendData(uint8_t ID, uint8_t *SendBuffer, uint32_t SendLength);

/**
 * @brief  Basic receive data routine.
 * @param  ID UART ID number.
 * @param  RecBuffer Pointer to store the received data.
 * @param  RecLength Maximum amount of data to receive.
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t UART_ReceiveData(uint8_t ID, uint8_t *RecBuffer, uint32_t RecMaxSize);

/**
 * @brief Reconfigure UART hardware with the specified parameters
 * @param  ID UART ID number.
 * @param  UART_Config Pointer to the desired UART's parameters
 * @retval ReturnCode_t
 */
ReturnCode_t UART_Reconfigure(uint8_t ID, UART_Config_t ConfigUART);

/**
 * @brief Set the UART to reset default values
 * @param  ID UART ID number.
 * @retval ReturnCode_t
 */
ReturnCode_t UART_DeInit(uint8_t ID);

#endif /* UART_H_ */
