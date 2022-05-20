/**
 ******************************************************************************
 * @file    UART.c
 * @author  Plinio Barbosa da Silva
 * @brief   UART module driver.
 * @brief   Version 2019.11.1
 * @brief   Creation Date 28/03/2018
 *
 *          This file provides functions to manage UART
 *          functionalities:
 *           + Initialization of a UART
 *           + Read data from UART
 *           + Write data to UART
 */
#include "UART.h"
#include "stm32f4xx.h"

#include "string.h"

#include "sys_cfg_stm32f407.h"

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

static UART_Config_t UART_DeviceList[UART_MAX_ID_LIMIT];

UART_Ctrl_Data_t UART1_Control;
UART_Ctrl_Data_t UART2_Control;
UART_Ctrl_Data_t UART3_Control;
UART_Ctrl_Data_t UART4_Control;
UART_Ctrl_Data_t UART5_Control;
UART_Ctrl_Data_t UART6_Control;

/* APB1 UART BRR Value              9600, 19200, 57600, 115200, 230400, 460800, 921600 */
uint32_t APB1_BRR_Value_42MHz[] = {
    0x1117,
    0x88C,
    0x2D9,
    0x016D,
    0x00B6,
    0x005B,
    0x002E
};

/* APB2 UART BRR Value              9600, 19200,   57600, 115200, 230400, 460800, 921600 */
uint32_t APB2_BRR_Value_84MHz[] = {
    0x222E,
    0x1117,
    0x05B2,
    0x02D9,
    0x016D,
    0x00B6,
    0x005B
};

void UART_Setup(USART_TypeDef * UART_Instance, uint32_t UART_BRR_Value);

/**
 * @brief  UART configuration routine.
 * @param  ID ID that should be allocated and configured.
 * @param  UART_Config Pointer to the desired UART's parameters
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t UART_Init(uint8_t ID, UART_Config_t ConfigUART) {

  ReturnCode_t ReturnCode = ANSWERED_REQUEST;

  UART_DeviceList[ID].RoutedPort = ConfigUART.RoutedPort;
  UART_DeviceList[ID].BaudRate = ConfigUART.BaudRate;
  UART_DeviceList[ID].ReceptionTimeOut_ms = ConfigUART.ReceptionTimeOut_ms;

  switch (UART_DeviceList[ID].RoutedPort) {

    case UART1_AT_PA10_PA9_RX_TX:
      /*
       * 1- Attach the alternate function.
       * 2- Enable the clock to the alternate function.
       * 3- Enable clock to corresponding GPIO.
       * 4- Configure the input-output port and pins (of the corresponding GPIOx) to match the AF .
       * 5- If desired enable the nested vector interrupt control to generate interrupts.
       * 6- Program the AF/peripheral for the required configuration (eg baud rate for a UART).
       */

      SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); // Enable GPIOA Clock

      CLR_GPIO_AFRH(GPIOA, 9);
      SET_GPIO_AFRH(GPIOA, 9, 7);
      CLR_GPIO_AFRH(GPIOA, 10);
      SET_GPIO_AFRH(GPIOA, 10, 7);

      CLR_GPIO_MODE(GPIOA, 10);
      CLR_GPIO_PULL(GPIOA, 10);
      CLR_GPIO_SPEED(GPIOA, 10);
      SET_GPIO_MODE(GPIOA, 10, GPIO_MODER_MODE_ALT_FUNC); // PA10 -> RX -> Input Pin
      SET_GPIO_PULL(GPIOA, 10, GPIO_PUPDR_PULL_UP); // Pull-up on PA10
      SET_GPIO_SPEED(GPIOA, 10, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      CLR_GPIO_MODE(GPIOA, 9);
      CLR_GPIO_PULL(GPIOA, 9);
      CLR_GPIO_SPEED(GPIOA, 9);
      SET_GPIO_MODE(GPIOA, 9, GPIO_MODER_MODE_ALT_FUNC); // PA10 -> RX -> Input Pin
      SET_GPIO_SPEED(GPIOA, 9, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      /* Enable UART1 Clock */
      SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);
      /* Adjust UART2 configurations */
      /*
       * STM32F407 UARTs
       * 1 and 6 are attached to APB2 -> 84MHz
       * 2 to 5 are attached to APB1  -> 42MHz
       */
      UART_Setup(USART1, APB2_BRR_Value_84MHz[UART_DeviceList[ID].BaudRate]);
      /* UART2 interrupt Init */

     //Tick_Config(SystemCoreClock/1000);
     //Tick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
      NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
      NVIC_EnableIRQ(USART1_IRQn);

      UART_DeviceList[ID].State = UART_STATE_IDLE;
      UART1_Control.Locked_ID = UART_NOT_BUSY;

      break;

    case UART2_AT_PD6_PD5_RX_TX:
      /*
       * 1- Attach the alternate function.
       * 2- Enable the clock to the alternate function.
       * 3- Enable clock to corresponding GPIO.
       * 4- Configure the input-output port and pins (of the corresponding GPIOx) to match the AF .
       * 5- If desired enable the nested vector interrupt control to generate interrupts.
       * 6- Program the AF/peripheral for the required configuration (eg baud rate for a UART).
       */

      SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // Enable GPIOD Clock

      CLR_GPIO_AFRL(GPIOD, 6);
      SET_GPIO_AFRL(GPIOD, 6, 7);
      CLR_GPIO_AFRL(GPIOD, 5);
      SET_GPIO_AFRL(GPIOD, 5, 7);

      CLR_GPIO_MODE(GPIOD,  6);
      CLR_GPIO_PULL(GPIOD,  6);
      CLR_GPIO_SPEED(GPIOD, 6);
      SET_GPIO_MODE(GPIOD,  6, GPIO_MODER_MODE_ALT_FUNC); // PD6 -> RX -> Input Pin
      SET_GPIO_PULL(GPIOD,  6, GPIO_PUPDR_PULL_UP); // Pull-up on PD6
      SET_GPIO_SPEED(GPIOD, 6, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      CLR_GPIO_MODE(GPIOD,  5);
      CLR_GPIO_PULL(GPIOD,  5);
      CLR_GPIO_SPEED(GPIOD, 5);
      SET_GPIO_MODE(GPIOD,  5, GPIO_MODER_MODE_ALT_FUNC); // PA10 -> RX -> Input Pin
      SET_GPIO_SPEED(GPIOD, 5, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      /* Enable UART2 Clock */
      SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);
      /* Adjust UART2 configurations */
      /*
       * STM32F407 UARTs
       * 1 and 6 are attached to APB2 -> 84MHz
       * 2 to 5 are attached to APB1  -> 42MHz
       */
      UART_Setup(USART2, APB1_BRR_Value_42MHz[UART_DeviceList[ID].BaudRate]);
      /* UART2 interrupt Init *////      SysTick_Config(SystemCoreClock/1000);

      NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
      NVIC_EnableIRQ(USART2_IRQn);


      UART_DeviceList[ID].State = UART_STATE_IDLE;
      UART2_Control.Locked_ID = UART_NOT_BUSY;

      break;

    case UART3_AT_PD9_PD8_RX_TX:
      /*
       * 1- Attach the alternate function.
       * 2- Enable the clock to the alternate function.
       * 3- Enable clock to corresponding GPIO.
       * 4- Configure the input-output port and pins (of the corresponding GPIOx) to match the AF .
       * 5- If desired enable the nested vector interrupt control to generate interrupts.
       * 6- Program the AF/peripheral for the required configuration (eg baud rate for a UART).
       */

      SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // Enable GPIOD Clock

      CLR_GPIO_AFRH(GPIOD, 9);
      SET_GPIO_AFRH(GPIOD, 9, 7);
      CLR_GPIO_AFRH(GPIOD, 8);
      SET_GPIO_AFRH(GPIOD, 8, 7);

      CLR_GPIO_MODE(GPIOD,  9);
      CLR_GPIO_PULL(GPIOD,  9);
      CLR_GPIO_SPEED(GPIOD, 9);
      SET_GPIO_MODE(GPIOD,  9, GPIO_MODER_MODE_ALT_FUNC); // PD9 -> RX -> Input Pin
      SET_GPIO_PULL(GPIOD,  9, GPIO_PUPDR_PULL_UP); // Pull-up on PD6
      SET_GPIO_SPEED(GPIOD, 9, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      CLR_GPIO_MODE(GPIOD,  8);
      CLR_GPIO_PULL(GPIOD,  8);
      CLR_GPIO_SPEED(GPIOD, 8);
      SET_GPIO_MODE(GPIOD,  8, GPIO_MODER_MODE_ALT_FUNC); // PD8 -> RX -> Input Pin
      SET_GPIO_SPEED(GPIOD, 8, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      /* Enable UART3 Clock */
      SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART3EN);
      /* Adjust UART3 configurations */
      /*
       * STM32F407 UARTs
       * 1 and 6 are attached to APB2 -> 84MHz
       * 2 to 5 are attached to APB1  -> 42MHz
       */
      UART_Setup(USART3, APB1_BRR_Value_42MHz[UART_DeviceList[ID].BaudRate]);
      /* UART3 interrupt Init */
//      SysTick_Config(SystemCoreClock/1000);
//      SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
      NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
      NVIC_EnableIRQ(USART3_IRQn);

      UART_DeviceList[ID].State = UART_STATE_IDLE;
      UART3_Control.Locked_ID = UART_NOT_BUSY;

      break;

    case UART6_AT_PC6_PC7_RX_TX:
      /*
       * 1- Attach the alternate function.
       * 2- Enable the clock to the alternate function.
       * 3- Enable clock to corresponding GPIO.
       * 4- Configure the input-output port and pins (of the corresponding GPIOx) to match the AF .
       * 5- If desired enable the nested vector interrupt control to generate interrupts.
       * 6- Program the AF/peripheral for the required configuration (eg baud rate for a UART).
       */

      SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // Enable GPIOC Clock

      CLR_GPIO_AFRL(GPIOC, 6);
      SET_GPIO_AFRL(GPIOC, 6, 8);
      CLR_GPIO_AFRL(GPIOC, 7);
      SET_GPIO_AFRL(GPIOC, 7, 8);

      CLR_GPIO_MODE(GPIOC,  6);
      CLR_GPIO_PULL(GPIOC,  6);
      CLR_GPIO_SPEED(GPIOC, 6);
      SET_GPIO_MODE(GPIOC,  6, GPIO_MODER_MODE_ALT_FUNC);      // PC6 -> RX -> Input Pin
      SET_GPIO_PULL(GPIOC,  6, GPIO_PUPDR_PULL_UP);            // Pull-up on PD6
      SET_GPIO_SPEED(GPIOC, 6, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      CLR_GPIO_MODE(GPIOC,  7);
      CLR_GPIO_PULL(GPIOC,  7);
      CLR_GPIO_SPEED(GPIOC, 7);
      SET_GPIO_MODE(GPIOC,  7, GPIO_MODER_MODE_ALT_FUNC);      // PC7 -> TX -> Input Pin
      SET_GPIO_SPEED(GPIOC, 7, GPIO_OSPEED_HIGH_SPEED_100MHZ); // Output High Speed

      /* Enable UART6 Clock */
      SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART6EN);
      /* Adjust UART6 configurations */
      /*
       * STM32F407 UARTs
       * 1 and 6 are attached to APB2 -> 84MHz
       * 2 to 5 are attached to APB1  -> 42MHz
       */
      UART_Setup(USART6, APB2_BRR_Value_84MHz[UART_DeviceList[ID].BaudRate]);
      /* UART6 interrupt Init */
//      SysTick_Config(SystemCoreClock/1000);
//      SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
      NVIC_SetPriority(USART6_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0, 0));
      NVIC_EnableIRQ(USART6_IRQn);

      UART_DeviceList[ID].State = UART_STATE_IDLE;
      UART6_Control.Locked_ID = UART_NOT_BUSY;

      break;
    default:
      ReturnCode = ERR_PARAM_ID;
      break;
  }
  return ReturnCode;
}

/**
 * @brief  Basic send data routine.
 * @param  ID UART ID number.
 * @param  SendBuffer Pointer to where the data is.
 * @param  SendLength How many bytes to send
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t UART_SendData(uint8_t ID, uint8_t * SendBuffer, uint32_t SendLength) {
  ReturnCode_t ReturnCode = OPERATION_RUNNING;

  switch (UART_DeviceList[ID].RoutedPort) {
    case UART1_AT_PA10_PA9_RX_TX:
      switch (UART_DeviceList[ID].State) {
        case UART_STATE_IDLE:

          ReturnCode = OPERATION_RUNNING;

          if (UART1_Control.Locked_ID == UART_NOT_BUSY) {
            //memcpy(UART1_Control.Transmit_Buffer, SendBuffer, SendLength);
        	  UART1_Control.Transmit_Buffer = SendBuffer;
            UART_DeviceList[ID].Tx_Size = SendLength;
            if ((USART1->SR & USART_SR_TC) == USART_SR_TC) {
              UART1_Control.Locked_ID = ID; // Lock the UART to this ID
              UART1_Control.Transmit_Counter = 0;
              USART1->DR = (*UART1_Control.Transmit_Buffer);
              UART1_Control.Transmit_Buffer ++;
              UART1_Control.Transmit_Counter++;
              SET_BIT(USART1->CR1, USART_CR1_TCIE);
              UART_DeviceList[ID].State = UART_STATE_TRANSMITING;
            }
          } else {
            ReturnCode = ERR_BUSY;
          }
          break;
        case UART_STATE_TRANSMITING:
          ReturnCode = OPERATION_RUNNING;
          if (UART1_Control.Locked_ID == ID) {
            if (UART1_Control.Transmit_Counter >= UART_DeviceList[ID].Tx_Size) {
              CLEAR_BIT(USART1->CR1, USART_CR1_TCIE);
              UART1_Control.Locked_ID = UART_NOT_BUSY;
              UART_DeviceList[ID].State = UART_STATE_IDLE;
              ReturnCode = ANSWERED_REQUEST;
            }
          } else {
            ReturnCode = ERR_BUSY;
          }
          break;
        default:
          break;
      }
      break;

        case UART2_AT_PD6_PD5_RX_TX:
          switch (UART_DeviceList[ID].State) {
            case UART_STATE_IDLE:

              ReturnCode = OPERATION_RUNNING;

              if (UART2_Control.Locked_ID == UART_NOT_BUSY) {
                //memcpy(UART2_Control.Transmit_Buffer, SendBuffer, SendLength);
            	UART2_Control.Transmit_Buffer = SendBuffer;
                UART_DeviceList[ID].Tx_Size = SendLength;
                if ((USART2->SR & USART_SR_TC) == USART_SR_TC) {
                  UART2_Control.Locked_ID = ID; // Lock the UART to this ID
                  UART2_Control.Transmit_Counter = 0;
                  USART2->DR = *UART2_Control.Transmit_Buffer;
                  UART2_Control.Transmit_Buffer ++;
                  UART2_Control.Transmit_Counter++;
                  SET_BIT(USART2->CR1, USART_CR1_TCIE);
                  UART_DeviceList[ID].State = UART_STATE_TRANSMITING;
                }
              } else {
                ReturnCode = ERR_BUSY;
              }
              break;
            case UART_STATE_TRANSMITING:
              ReturnCode = OPERATION_RUNNING;
              if (UART2_Control.Locked_ID == ID) {
                if (UART2_Control.Transmit_Counter >= UART_DeviceList[ID].Tx_Size) {
                  CLEAR_BIT(USART2->CR1, USART_CR1_TCIE);
                  UART2_Control.Locked_ID = UART_NOT_BUSY;
                  UART_DeviceList[ID].State = UART_STATE_IDLE;
                  ReturnCode = ANSWERED_REQUEST;
                }
              } else {
                ReturnCode = ERR_BUSY;
              }
              break;
            default:
              break;
          }
          break;

            case UART3_AT_PD9_PD8_RX_TX:
              switch (UART_DeviceList[ID].State) {
                case UART_STATE_IDLE:

                  ReturnCode = OPERATION_RUNNING;

                  if (UART3_Control.Locked_ID == UART_NOT_BUSY) {
                    //memcpy(UART3_Control.Transmit_Buffer, SendBuffer, SendLength);
                	UART3_Control.Transmit_Buffer = SendBuffer;
                    UART_DeviceList[ID].Tx_Size = SendLength;
                    if ((USART3->SR & USART_SR_TC) == USART_SR_TC) {
                      UART3_Control.Locked_ID = ID; // Lock the UART to this ID
                      UART3_Control.Transmit_Counter = 0;
                      USART3->DR = *UART3_Control.Transmit_Buffer;
                      UART3_Control.Transmit_Buffer ++;
                      UART3_Control.Transmit_Counter++;
                      SET_BIT(USART3->CR1, USART_CR1_TCIE);
                      UART_DeviceList[ID].State = UART_STATE_TRANSMITING;
                    }
                  } else {
                    ReturnCode = ERR_BUSY;
                  }
                  break;
                case UART_STATE_TRANSMITING:
                  ReturnCode = OPERATION_RUNNING;
                  if (UART3_Control.Locked_ID == ID) {
                    if (UART3_Control.Transmit_Counter >= UART_DeviceList[ID].Tx_Size) {
                      CLEAR_BIT(USART3->CR1, USART_CR1_TCIE);
                      UART3_Control.Locked_ID = UART_NOT_BUSY;
                      UART_DeviceList[ID].State = UART_STATE_IDLE;
                      ReturnCode = ANSWERED_REQUEST;
                    }
                  } else {
                    ReturnCode = ERR_BUSY;
                  }
                  break;
                default:
                  break;
              }
              break;

                case UART6_AT_PC6_PC7_RX_TX:
                  switch (UART_DeviceList[ID].State) {
                    case UART_STATE_IDLE:

                      ReturnCode = OPERATION_RUNNING;

                      if (UART6_Control.Locked_ID == UART_NOT_BUSY) {
                        //memcpy(UART6_Control.Transmit_Buffer, SendBuffer, SendLength);
                    	UART6_Control.Transmit_Buffer = SendBuffer;
                        UART_DeviceList[ID].Tx_Size = SendLength;
                        if ((USART6->SR & USART_SR_TC) == USART_SR_TC) {
                          UART6_Control.Locked_ID = ID; // Lock the UART to this ID
                          UART6_Control.Transmit_Counter = 0;
                          USART6->DR = *UART6_Control.Transmit_Buffer;
                          UART6_Control.Transmit_Buffer ++;
                          UART6_Control.Transmit_Counter++;
                          SET_BIT(USART6->CR1, USART_CR1_TCIE);
                          UART_DeviceList[ID].State = UART_STATE_TRANSMITING;
                        }
                      } else {
                        ReturnCode = ERR_BUSY;
                      }
                      break;
                    case UART_STATE_TRANSMITING:
                      ReturnCode = OPERATION_RUNNING;
                      if (UART6_Control.Locked_ID == ID) {
                        if (UART6_Control.Transmit_Counter >= UART_DeviceList[ID].Tx_Size) {
                          CLEAR_BIT(USART6->CR1, USART_CR1_TCIE);
                          UART6_Control.Locked_ID = UART_NOT_BUSY;
                          UART_DeviceList[ID].State = UART_STATE_IDLE;
                          ReturnCode = ANSWERED_REQUEST;
                        }
                      } else {
                        ReturnCode = ERR_BUSY;
                      }
                      break;
                    default:
                      break;
                  }
                  break;
                    default:
                      ReturnCode = ERR_PARAM_ID;
                      break;
  }
  return ReturnCode;
}

/**
 * @brief  Basic receive data routine.
 * @param  ID UART ID number.
 * @param  RecBuffer Pointer to store the received data.
 * @param  RecLength Maximum amount of data to receive.
 * @note
 * @retval ReturnCode_t
 */
ReturnCode_t UART_ReceiveData(uint8_t ID, uint8_t * RecBuffer, uint32_t RecMaxSize) {
  ReturnCode_t ReturnCode;
  uint32_t TimeStamp;
  uint32_t Inter_Caracter_Time;

  ReturnCode = OPERATION_RUNNING;

  switch (UART_DeviceList[ID].RoutedPort) {
    case UART1_AT_PA10_PA9_RX_TX:
      switch (UART_DeviceList[ID].State) {
        case UART_STATE_IDLE:
          if (UART1_Control.Locked_ID == UART_NOT_BUSY) {
            UART1_Control.Receive_Counter = 0;
            UART1_Control.Receive_Buffer = RecBuffer;
            UART1_Control.Receive_Buffer += 2;//UART_DeviceList[ID].Rx_Buffer = RecBuffer;
            //CLEAR_BIT(USART1->SR, USART_SR_RXNE);
            SET_BIT(USART1->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable
            UART_DeviceList[ID].Rx_Size = RecMaxSize;
            UART_DeviceList[ID].State = UART_STATE_RECEIVING;
            UART1_Control.Locked_ID = ID; // Lock the UART to this ID
          }
          break;
        case UART_STATE_RECEIVING:
          if (UART1_Control.Locked_ID == ID) {
            if(RecMaxSize == UART_ABORT_RECEPTION)
            {
              CLEAR_BIT(USART1->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
              UART1_Control.Receive_Counter = 0;
              UART_DeviceList[ID].State = UART_STATE_IDLE;
              UART1_Control.Locked_ID = UART_NOT_BUSY;
              ReturnCode = ANSWERED_REQUEST;
            }
            if (UART1_Control.Receive_Counter > 0)
            {
              TimeStamp = Sys_Get_Tick();
              Inter_Caracter_Time = TimeStamp - UART1_Control.Last_Receive_Timestamp;

              if ((Inter_Caracter_Time > UART_DeviceList[ID].ReceptionTimeOut_ms) || (UART1_Control.Receive_Counter >= UART_DeviceList[ID].Rx_Size))
              {
                CLEAR_BIT(USART1->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                *(UART1_Control.Receive_Buffer - (UART1_Control.Receive_Counter + 2))        = UART1_Control.Receive_Counter & 0xFF;
                *(UART1_Control.Receive_Buffer - (UART1_Control.Receive_Counter + 1))        =(UART1_Control.Receive_Counter >> 8) & 0xFF;
                //UART1_Control.Receive_Buffer[0] = UART1_Control.Receive_Counter & 0xFF;
                //UART1_Control.Receive_Buffer[1] = (UART1_Control.Receive_Counter >> 8) & 0xFF;
                //UART_DeviceList[ID].Rx_Buffer[0] = UART1_Control.Receive_Counter & 0xFF;
                //UART_DeviceList[ID].Rx_Buffer[1] = (UART1_Control.Receive_Counter >> 8) & 0xFF;
                //memcpy(UART_DeviceList[ID].Rx_Buffer + 2, UART1_Control.Receive_Buffer, UART1_Control.Receive_Counter);
                UART_DeviceList[ID].State = UART_STATE_IDLE;
                UART1_Control.Locked_ID = UART_NOT_BUSY;
                ReturnCode = ANSWERED_REQUEST;
              }
            }


          }
          break;
        default:
          break;
      }
      break;

        case UART2_AT_PD6_PD5_RX_TX:
          switch (UART_DeviceList[ID].State) {
            case UART_STATE_IDLE:
              if (UART2_Control.Locked_ID == UART_NOT_BUSY) {
                UART2_Control.Receive_Counter 	= 0;
                UART2_Control.Receive_Buffer 	= RecBuffer;
                UART2_Control.Receive_Buffer 	+= 2;//UART_DeviceList[ID].Rx_Buffer = RecBuffer;
                //UART_DeviceList[ID].Rx_Buffer = RecBuffer;
                //CLEAR_BIT(USART2->SR, USART_SR_RXNE);
                SET_BIT(USART2->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable
                UART_DeviceList[ID].Rx_Size = RecMaxSize;
                UART_DeviceList[ID].State = UART_STATE_RECEIVING;
                UART2_Control.Locked_ID = ID; // Lock the UART to this ID
              }
              break;
            case UART_STATE_RECEIVING:
              if (UART2_Control.Locked_ID == ID)
              {
                if(RecMaxSize == UART_ABORT_RECEPTION)
                {
                  CLEAR_BIT(USART2->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                  UART2_Control.Receive_Counter = 0;
                  UART_DeviceList[ID].State = UART_STATE_IDLE;
                  UART2_Control.Locked_ID = UART_NOT_BUSY;
                  ReturnCode = ANSWERED_REQUEST;
                }
                if (UART2_Control.Receive_Counter > 0)
                {
                  TimeStamp = Sys_Get_Tick();
                  Inter_Caracter_Time = TimeStamp - UART2_Control.Last_Receive_Timestamp;

                  if ((Inter_Caracter_Time > UART_DeviceList[ID].ReceptionTimeOut_ms) ||
                      (UART2_Control.Receive_Counter >= UART_DeviceList[ID].Rx_Size)) {
                    CLEAR_BIT(USART2->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                    //UART2_Control.Receive_Buffer[0] = UART2_Control.Receive_Counter & 0xFF;
                    //UART2_Control.Receive_Buffer[1] = (UART2_Control.Receive_Counter >> 8) & 0xFF;
                    *(UART2_Control.Receive_Buffer - (UART2_Control.Receive_Counter + 2))        = UART2_Control.Receive_Counter & 0xFF;
                    *(UART2_Control.Receive_Buffer - (UART2_Control.Receive_Counter + 1))        =(UART2_Control.Receive_Counter >> 8) & 0xFF;
                    //UART_DeviceList[ID].Rx_Buffer[0] = UART2_Control.Receive_Counter & 0xFF;
                    //UART_DeviceList[ID].Rx_Buffer[1] = (UART2_Control.Receive_Counter >> 8) & 0xFF;
                    //memcpy(UART_DeviceList[ID].Rx_Buffer + 2, UART2_Control.Receive_Buffer, UART2_Control.Receive_Counter);
                    UART_DeviceList[ID].State = UART_STATE_IDLE;
                    UART2_Control.Locked_ID = UART_NOT_BUSY;
                    ReturnCode = ANSWERED_REQUEST;
                  }
                }
              }
              break;
            default:
              break;
          }
          break;

            case UART3_AT_PD9_PD8_RX_TX:
              switch (UART_DeviceList[ID].State) {
                case UART_STATE_IDLE:
                  if (UART3_Control.Locked_ID == UART_NOT_BUSY) {
                    UART3_Control.Receive_Counter 	= 0;
                    UART3_Control.Receive_Buffer 	= RecBuffer;
                    UART3_Control.Receive_Buffer 	+= 2;
                    //UART_DeviceList[ID].Rx_Buffer = RecBuffer;
                    //CLEAR_BIT(USART3->SR, USART_SR_RXNE);
                    SET_BIT(USART3->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable
                    UART_DeviceList[ID].Rx_Size = RecMaxSize;
                    UART_DeviceList[ID].State = UART_STATE_RECEIVING;
                    UART3_Control.Locked_ID = ID; // Lock the UART to this ID
                  }
                  break;
                case UART_STATE_RECEIVING:
                  if (UART3_Control.Locked_ID == ID) {
                    if(RecMaxSize == UART_ABORT_RECEPTION)
                    {
                      CLEAR_BIT(USART3->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                      UART3_Control.Receive_Counter = 0;
                      UART_DeviceList[ID].State = UART_STATE_IDLE;
                      UART3_Control.Locked_ID = UART_NOT_BUSY;
                      ReturnCode = ANSWERED_REQUEST;
                    }
                    if (UART3_Control.Receive_Counter > 0) {
                      TimeStamp = Sys_Get_Tick();
                      Inter_Caracter_Time = TimeStamp - UART3_Control.Last_Receive_Timestamp;

                      if ((Inter_Caracter_Time > UART_DeviceList[ID].ReceptionTimeOut_ms) || (UART3_Control.Receive_Counter >= UART_DeviceList[ID].Rx_Size)) {
                        CLEAR_BIT(USART3->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                        *(UART3_Control.Receive_Buffer - (UART3_Control.Receive_Counter + 2))        = UART3_Control.Receive_Counter & 0xFF;
                        *(UART3_Control.Receive_Buffer - (UART3_Control.Receive_Counter + 1))        =(UART3_Control.Receive_Counter >> 8) & 0xFF;
                        //UART_DeviceList[ID].Rx_Buffer[0] = UART3_Control.Receive_Counter & 0xFF;
                        //UART_DeviceList[ID].Rx_Buffer[1] = (UART3_Control.Receive_Counter >> 8) & 0xFF;
                        //memcpy(UART_DeviceList[ID].Rx_Buffer + 2, UART3_Control.Receive_Buffer, UART3_Control.Receive_Counter);
                        UART_DeviceList[ID].State = UART_STATE_IDLE;
                        UART3_Control.Locked_ID = UART_NOT_BUSY;
                        ReturnCode = ANSWERED_REQUEST;
                      }
                    }
                  }
                  break;
                default:
                  break;
              }
              break;

                case UART6_AT_PC6_PC7_RX_TX:
                  switch (UART_DeviceList[ID].State) {
                    case UART_STATE_IDLE:
                      if (UART6_Control.Locked_ID == UART_NOT_BUSY) {
                        UART6_Control.Receive_Counter   = 0;
                        UART6_Control.Receive_Buffer 	= RecBuffer;
                        UART6_Control.Receive_Buffer 	+= 2;
                        //UART_DeviceList[ID].Rx_Buffer = RecBuffer;
                        //CLEAR_BIT(USART6->SR, USART_SR_RXNE);
                        SET_BIT(USART6->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable
                        UART_DeviceList[ID].Rx_Size = RecMaxSize;
                        UART_DeviceList[ID].State = UART_STATE_RECEIVING;
                        UART6_Control.Locked_ID = ID; // Lock the UART to this ID
                      }
                      break;
                    case UART_STATE_RECEIVING:
                      if (UART6_Control.Locked_ID == ID) {
                        if(RecMaxSize == UART_ABORT_RECEPTION)
                        {
                          CLEAR_BIT(USART6->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                          UART6_Control.Receive_Counter = 0;
                          UART_DeviceList[ID].State = UART_STATE_IDLE;
                          UART6_Control.Locked_ID = UART_NOT_BUSY;
                          ReturnCode = ANSWERED_REQUEST;
                        }
                        if (UART6_Control.Receive_Counter > 0) {
                          TimeStamp = Sys_Get_Tick();
                          Inter_Caracter_Time = TimeStamp - UART6_Control.Last_Receive_Timestamp;

                          if ((Inter_Caracter_Time > UART_DeviceList[ID].ReceptionTimeOut_ms) || (UART6_Control.Receive_Counter >= UART_DeviceList[ID].Rx_Size)) {
                            CLEAR_BIT(USART6->CR1, USART_CR1_RXNEIE); // Bit 5 RXNEIE: RXNE interrupt enable)
                            *(UART6_Control.Receive_Buffer - (UART6_Control.Receive_Counter + 2))        = UART6_Control.Receive_Counter & 0xFF;
                            *(UART6_Control.Receive_Buffer - (UART6_Control.Receive_Counter + 1))        =(UART6_Control.Receive_Counter >> 8) & 0xFF;
                            //UART6_Control.Receive_Buffer[0] = UART6_Control.Receive_Counter & 0xFF;
                            //UART6_Control.Receive_Buffer[1] = (UART6_Control.Receive_Counter >> 8) & 0xFF;
                            //UART_DeviceList[ID].Rx_Buffer[0] = UART6_Control.Receive_Counter & 0xFF;
                            //UART_DeviceList[ID].Rx_Buffer[1] = (UART6_Control.Receive_Counter >> 8) & 0xFF;
                            //memcpy(UART_DeviceList[ID].Rx_Buffer + 2, UART6_Control.Receive_Buffer, UART6_Control.Receive_Counter);
                            UART_DeviceList[ID].State = UART_STATE_IDLE;
                            UART6_Control.Locked_ID = UART_NOT_BUSY;
                            ReturnCode = ANSWERED_REQUEST;
                          }
                        }
                      }
                      break;
                    default:
                      break;
                  }
                  break;
                    default:
                      ReturnCode = ERR_PARAM_ID;
                      break;
  }

  return ReturnCode;
}

/**
 * @brief Reconfigure UART hardware with the specified parameters
 * @param  ID UART ID number.
 * @param  UART_Config Pointer to the desired UART's parameters
 * @retval ReturnCode_t
 */
ReturnCode_t UART_Reconfigure(uint8_t ID, UART_Config_t ConfigUART)
{
	ReturnCode_t RetCode = ANSWERED_REQUEST;

	return RetCode;
}

/**
 * @brief Set the UART to reset default values
 * @param  ID UART ID number.
 * @retval ReturnCode_t
 */
ReturnCode_t UART_DeInit(uint8_t ID)
{
	ReturnCode_t RetCode = ANSWERED_REQUEST;

	return RetCode;
}

void UART_Setup(USART_TypeDef * UART_Instance, uint32_t UART_BRR_Value) {
  /* STM32F407 UARTs
   * 1 and 6 are attached to APB2 -> 84MHz
   * 2 to 5 are attached to APB1  -> 42MHz
   */

  /* To calculate the baud rate use: baud = Fck / (16*UARTDIV)
   * Where:
   * Fck - Input clock to the peripheral (PCLK1 for UART2, 3, 4, 5 or PCLK2 for UART1,6
   * UARTDIV is an unsigned fixed point number that is coded on the UART_BRR register.
   *
   * Example for 11200bps on 42MHz:
   * BRR = fclk / (16 x Baud) = 42MHz / (16 x 115200) = 22,7865 decimal
   * DIV_Mantissa = 22 dec = 0x16
   * DIV_Fraction = 0.7865 dec = 16 x 0.7865 = 12,584 ~ 13 = 0x0D
   * UART_BRR = 0x16D
   */

  UART_Instance->BRR = UART_BRR_Value;

  UART_Instance->CR1 &= ~(USART_CR1_M); //~(1<<12); //Bit 12 M: Word length: -> 1 Start bit, 8 Data bits, n Stop bit
  UART_Instance->CR1 &= ~(USART_CR1_PCE | USART_CR1_PS); //~(3<<9);  //Bit 10 PCE: Parity control enable And  Bit 9 PS: Parity selection -> All disable
  UART_Instance->CR2 &= ~(USART_CR2_STOP); //~(3<<12); //Bits 13:12 STOP: STOP bits -> 1 Stop bit
  UART_Instance->CR3 &= ~(USART_CR3_CTSIE | USART_CR3_CTSE | USART_CR3_RTSE); //~(3<<8);  //Bit 10:09:08 CTSIE:CTSE:RTSE -> No flow control

  UART_Instance->CR1 |= (USART_CR1_TE | USART_CR1_RE); //3<<2;   // Bits 03:02 TE:RE -> RX, TX enable
  UART_Instance->CR1 |= (USART_CR1_UE); //1<<13;   // Bit 13 UE: USART enable -> USART enable

  //USART_Instance->CR1 |= (USART_CR1_RXNEIE);//1 << 5;   // Bit 5 RXNEIE: RXNE interrupt enable
  //USART2->CR1 |= (USART_CR1_TXEIE);//1 << 7;   // Bit 7 TXEIE: TXE interrupt enable
}
