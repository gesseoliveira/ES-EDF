
#include "GPIO.h"
#include "returncode.h"
#include "string.h"
#include "stm32f4xx.h"
#include "types.h"

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

#define GPIO_SET_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) |=  (1UL << (BIT_NUM)))
#define GPIO_CLR_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) &= ~(1UL << (BIT_NUM)))
#define GPIO_TGL_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) ^=  (1UL << (BIT_NUM)))
#define GPIO_GET_REG_BIT(REGISTER, BIT_NUM) ((REGISTER) &   (1UL << (BIT_NUM)))

/*Enable clock ports*/
#define GPIOA_CLK_ENABLE()	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN)

#define GPIOB_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN)

#define GPIOC_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN)

#define GPIOD_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN)

#define GPIOE_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN)

#define GPIOF_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN)

#define GPIOG_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN)

#define GPIOH_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN)

#define GPIOI_CLK_ENABLE()  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN)

// Internal
typedef struct
{
  GPIO_TypeDef     *GPIO_Port;      /*!< Specifies the GPIO port to be used. */
  uint32_t          GPIO_Pin;       /*!< Specifies the GPIO pin to be used. */
  GPIO_Direction_t  GPIO_Direction; /*!< Specifies the pin direction @ref GPIO_Direction_t. */
  GPIO_Speed_t      GPIO_Speed;     /*!< Specifies the pin speed @ref GPIO_Speed_t. */
  uint8_t           Initialized;
} GPIO_Parameters_Interal_t;

static GPIO_Parameters_Interal_t GPIO_List[GPIO_MAX_AVAILABLE_ID];

/**
 * @brief  Pin configuration routine.
 * @param  ID : ID that should be allocated and configured.
 * @param  Parameter : Desired pin's configuration parameters. See @ref GPIO_Parameters_t
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_Init(uint8_t ID, GPIO_Parameters_t Parameter)
{
  ReturnCode_t  ReturnValue = ANSWERED_REQUEST;

  if(ID < GPIO_MAX_AVAILABLE_ID)
  {
    if(GPIO_List[ID].Initialized == FALSE)
    {
      if(GPIO_List[ID].GPIO_Pin <= 15)
      {
        GPIO_List[ID].GPIO_Pin        = Parameter.GPIO_Pin;
        GPIO_List[ID].GPIO_Speed      = Parameter.GPIO_Speed;
        GPIO_List[ID].GPIO_Direction  = Parameter.GPIO_Direction;

        switch(Parameter.GPIO_Port)
        {
        case  GPIO_PORT_A:
          GPIO_List[ID].GPIO_Port  = GPIOA;
          break;

        case  GPIO_PORT_B:
          GPIO_List[ID].GPIO_Port  = GPIOB;
          break;

        case  GPIO_PORT_C:
          GPIO_List[ID].GPIO_Port  = GPIOC;
          break;

        case  GPIO_PORT_D:
          GPIO_List[ID].GPIO_Port  = GPIOD;
          break;

        case  GPIO_PORT_E:
          GPIO_List[ID].GPIO_Port  = GPIOE;
          break;

        case  GPIO_PORT_F:
          GPIO_List[ID].GPIO_Port  = GPIOF;
          break;

        case  GPIO_PORT_G:
          GPIO_List[ID].GPIO_Port  = GPIOG;
          break;

        case  GPIO_PORT_H:
          GPIO_List[ID].GPIO_Port  = GPIOH;
          break;
        }

        GPIOA_CLK_ENABLE();
        GPIOB_CLK_ENABLE();
        GPIOC_CLK_ENABLE();
        GPIOD_CLK_ENABLE();
        GPIOE_CLK_ENABLE();
        GPIOF_CLK_ENABLE();
        GPIOG_CLK_ENABLE();
        GPIOH_CLK_ENABLE();
        GPIOI_CLK_ENABLE();

        switch (GPIO_List[ID].GPIO_Direction)
        {
        case GPIO_MODE_IN_PULL_UP:
          // Set the MODER Reg to input mode 0x00
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          // Set the PUPDR Reg to Pull-up 0x01
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->PUPDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->PUPDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));

          break;
        case GPIO_MODE_IN_PULL_DOWN:
          // Set the MODER Reg to input mode 0x00
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          // Set the PUPDR Reg to Pull-down 0x10
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->PUPDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->PUPDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));

          break;
        case GPIO_MODE_IN_NO_PULL:
          // Set the MODER Reg to input mode 0x00
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          // Set the PUPDR Reg to no Pull-up or Pull-down 0x00
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->PUPDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->PUPDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          break;
        case GPIO_MODE_OUT_PP:
          // Set the MODER Reg to Output mode 0x00
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          // Set the OTYPER Reg to Output Push-Pull mode 0x0
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->OTYPER, GPIO_List[ID].GPIO_Pin );
          break;
        case GPIO_MODE_OUT_OD:
          // Set the MODER Reg to Output mode 0x00
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          // Set the OTYPER Reg to Output Open-Drain mode 0x1
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->OTYPER, GPIO_List[ID].GPIO_Pin );
          break;
        case GPIO_MODE_ANA:
          // Set the MODER Reg to analog mode 0x11
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->MODER, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          break;
        default:
          ReturnValue = ERR_PARAM_MODE;
          break;
        }

        switch (GPIO_List[ID].GPIO_Speed)
        {
        case GPIO_SPEED_PIN_SLOW:
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          break;
        case GPIO_SPEED_PIN_MEDIUM:
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          break;
        case GPIO_SPEED_PIN_HIGH:
          GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          break;
        case GPIO_SPEED_PIN_VERY_HIGH:
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ( GPIO_List[ID].GPIO_Pin * 2));
          GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->OSPEEDR, ((GPIO_List[ID].GPIO_Pin * 2) + 1));
          break;
        default:
          ReturnValue = ERR_PARAM_MODE;
          break;
        }
        GPIO_List[ID].Initialized = TRUE;
      }
      else
      {
        ReturnValue = ERR_PARAM_LENGTH;
      }
    }
    else
    {
      ReturnValue = ERR_ENABLED;
    }
  }
  else
  {
    ReturnValue = ERR_PARAM_ID;
  }
  return ReturnValue;
}

/**
 * @brief  Output set routine.
 * @param  ID : ID whose pin's should be set.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, time is being counted
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_SetOutput(uint8_t ID)
{
  ReturnCode_t ReturnValue= ANSWERED_REQUEST;
  if(ID < GPIO_MAX_AVAILABLE_ID)
  {
    if(GPIO_List[ID].GPIO_Pin <= 15)
      GPIO_SET_REG_BIT(GPIO_List[ID].GPIO_Port->ODR, GPIO_List[ID].GPIO_Pin);
    else
      ReturnValue = ERR_PARAM_LENGTH;
  }
  else
  {
    ReturnValue = ERR_PARAM_ID;
  }
  return ReturnValue;
}

/**
 * @brief  Output clear routine.
 * @param  ID : ID whose pin's should be cleared.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, pin's level has been cleared.
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_ClearOutput(uint8_t ID)
{ 
  ReturnCode_t ReturnValue= ANSWERED_REQUEST;
  if(ID < GPIO_MAX_AVAILABLE_ID)
  {
    if(GPIO_List[ID].GPIO_Pin <= 15)
      GPIO_CLR_REG_BIT(GPIO_List[ID].GPIO_Port->ODR, GPIO_List[ID].GPIO_Pin);
    else
      ReturnValue = ERR_PARAM_LENGTH;
  }
  else
  {
    ReturnValue = ERR_PARAM_ID;
  }
  return ReturnValue;
}

/**
 * @brief  Read pin routine.
 * @param  ID : ID whose pin's should be read.
 * @param  InputValue: Pointer to a variable where the read result will be stored.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, pin's level has been read.
 *             @arg Else: Some error happened.
 */
ReturnCode_t GPIO_ReadInput(uint8_t ID, uint8_t * InputValue)
{
  ReturnCode_t ReturnValue= ANSWERED_REQUEST;

  if(ID < GPIO_MAX_AVAILABLE_ID)
  {
    if(GPIO_List[ID].GPIO_Pin <= 15)
    {
      if(GPIO_GET_REG_BIT(GPIO_List[ID].GPIO_Port->IDR, GPIO_List[ID].GPIO_Pin))
        *InputValue = GPIO_SET;
      else
        *InputValue = GPIO_CLEAR;
    }
    else
    {
      ReturnValue = ERR_PARAM_LENGTH;
    }
  }
  else
  {
    ReturnValue = ERR_PARAM_ID;
  }
  return ReturnValue;
}

/**
 * @brief  Output toggle routine.
 * @param  ID : ID whose pin's should be toggled.
 * @retval Result : Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok, pin's level has been toggled.
 *             @arg Else: Some error happened.
 */
ReturnCode_t  GPIO_ToggleOutput(uint8_t ID)
{
  ReturnCode_t ReturnValue= ANSWERED_REQUEST;
  if(ID < GPIO_MAX_AVAILABLE_ID)
  {
    if(GPIO_List[ID].GPIO_Pin <= 15)
      GPIO_TGL_REG_BIT(GPIO_List[ID].GPIO_Port->ODR, GPIO_List[ID].GPIO_Pin);
    else
      ReturnValue = ERR_PARAM_LENGTH;
  }
  else
  {
    ReturnValue = ERR_PARAM_ID;
  }
  return ReturnValue;
}

/**
 * @brief  Reconfigure GPIO hardware with the specified parameters
 * @param  ID GPIO ID number.
 * @param  GPIO_Config Pointer to the desired GPIO's parameters
 * @retval ReturnCode_t
 */
ReturnCode_t GPIO_Reconfigure(uint8_t ID, GPIO_Parameters_t Parameter)
{
  ReturnCode_t RetCode = ANSWERED_REQUEST;
  GPIO_DeInit(ID);
  GPIO_Init(ID, Parameter);
  return RetCode;
}

/**
 * @brief  Set the GPIO to reset default values
 * @param  ID GPIO ID number.
 * @retval ReturnCode_t
 */
ReturnCode_t GPIO_DeInit(uint8_t ID)
{
  ReturnCode_t ReturnValue = ANSWERED_REQUEST;

  if(ID < GPIO_MAX_AVAILABLE_ID)
  {
    if(GPIO_List[ID].GPIO_Pin <= 15)
    {
      /*------------------------- GPIO Mode Configuration --------------------*/
      /* Configure IO Direction in Input Floating Mode */
      GPIO_List[ID].GPIO_Port->MODER &= ~(GPIO_MODER_MODER0 << (GPIO_List[ID].GPIO_Pin * 2U));

      /* Configure the default Alternate Function in current IO */
      GPIO_List[ID].GPIO_Port->AFR[GPIO_List[ID].GPIO_Pin >> 3U] &= ~(0xFU << ((uint32_t)(GPIO_List[ID].GPIO_Pin & 0x07U) * 4U)) ;

      /* Configure the default value for IO Speed */
      GPIO_List[ID].GPIO_Port->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 << (GPIO_List[ID].GPIO_Pin * 2U));

      /* Configure the default value IO Output Type */
      GPIO_List[ID].GPIO_Port->OTYPER  &= ~(GPIO_OTYPER_OT_0 << GPIO_List[ID].GPIO_Pin) ;

      /* Deactivate the Pull-up and Pull-down resistor for the current IO */
      GPIO_List[ID].GPIO_Port->PUPDR &= ~(GPIO_PUPDR_PUPDR0 << (GPIO_List[ID].GPIO_Pin * 2U));

      GPIO_List[ID].Initialized = FALSE;
    }
    else
    {
      ReturnValue = ERR_PARAM_LENGTH;
    }
  }
  else
  {
    ReturnValue = ERR_PARAM_ID;
  }

  return ReturnValue;
}
