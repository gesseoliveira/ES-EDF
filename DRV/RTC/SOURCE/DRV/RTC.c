/**
 *******************************************************************************
 * @file    RTC.c
 * @author  Peterson Aguiar
 * @brief   RTC Internal module driver.
 * @brief   Version 2019.9.1
 * @brief   Creation Date 23/07/2019
 *******************************************************************************
 */

#include "RTC.h"
#include "stm32f4xx.h"
#include "time.h"
#include "string.h"
#include "CustoMath.h"


/** @defgroup RTC masks
 * @{
 */
#define RTC_INIT_MASK           0xFFFFFFFFU
#define RTC_TR_RESERVED_MASK    0x007F7F7FU
#define RTC_DR_RESERVED_MASK    0x00FFFF3FU
#define RTC_RSF_MASK            0xFFFFFF5FU
/**
 * @}
 */

/** @defgroup RTC_Hour_Formats RTC Hour Formats
 * @{
 */
#define RTC_HOURFORMAT_24              0x00000000U
#define RTC_HOURFORMAT_12              0x00000040U
/**
 * @}
 */

/** @defgroup RTC_Output_selection_Definitions RTC Output Selection Definitions
 * @{
 */
#define RTC_OUTPUT_DISABLE             0x00000000U
#define RTC_OUTPUT_ALARMA              0x00200000U
#define RTC_OUTPUT_ALARMB              0x00400000U
#define RTC_OUTPUT_WAKEUP              0x00600000U
/**
 * @}
 */

/** @defgroup RTC_Output_Polarity_Definitions RTC Output Polarity Definitions
 * @{
 */
#define RTC_OUTPUT_POLARITY_HIGH       0x00000000U
#define RTC_OUTPUT_POLARITY_LOW        0x00100000U
/**
 * @}
 */

/** @defgroup RTC_Output_Type_ALARM_OUT RTC Output Type ALARM OUT
 * @{
 */
#define RTC_OUTPUT_TYPE_OPENDRAIN      0x00000000U
#define RTC_OUTPUT_TYPE_PUSHPULL       0x00040000U
/**
 * @}
 */

static bool RTC_EvaluateDateTime(DateTime_t *DateTimePtr);

bool          RTC_Initialized = FALSE;
bool          RTC_ClockIsRunning = FALSE;

AlarmArray_t  ListOfAlarms[RTC_MAX_ALARMS];
uint32        RtcInterruptCount = 0;

TimeStamp_t   RawTimeCount_CurrentValue = 0;
TimeStamp_t   RawTimeCount_LastSecondsValue = 0;

/**
 * @brief  Initialize the internal RTC.
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: Initialization Complete.
 *             @arg ERR_ENABLED: Internal RTC is already operating.
 * @note   This routine initializes the RTC driver and required variables.
 *
 */
ReturnCode_t RTC_Init(void)
{
  ReturnCode_t ReturnValue;
  uint8 currAlarm;

  if (RTC_Initialized)
  {
    ReturnValue = ERR_ENABLED;
  }
  else
  {
    RTC_Initialized = TRUE;

    /* Disable the write protection for RTC registers */
    RTC->WPR = 0xCAU;
    RTC->WPR = 0x53U;

    /* Check if the Initialization mode is set */
    if((RTC->ISR & RTC_ISR_INITF) == (uint32_t)RESET)
    {
      /* Set the Initialization mode */
      RTC->ISR = (uint32_t)RTC_INIT_MASK;
      /* Wait till RTC is in INIT state */
      while((RTC->ISR & RTC_ISR_INITF) == (uint32_t)RESET);
    }

    /* Clear RTC_CR FMT, OSEL and POL Bits */
    RTC->CR &= ((uint32_t)~(RTC_CR_FMT | RTC_CR_OSEL | RTC_CR_POL));
    /* Set RTC_CR register */
    RTC->CR |= (uint32_t)(RTC_HOURFORMAT_24 | RTC_OUTPUT_DISABLE | RTC_OUTPUT_POLARITY_HIGH);

    /* Configure the RTC PRER */
    RTC->PRER = (uint32_t)(255);
    RTC->PRER |= (uint32_t)(127 << 16U);

    /* Exit Initialization mode */
    RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

    if((RTC->CR & RTC_CR_BYPSHAD) == RESET)
    {
      /* Clear RSF flag */
      RTC->ISR &= (uint32_t)RTC_RSF_MASK;

      /* Wait the registers to be synchronised */
      while((RTC->ISR & RTC_ISR_RSF) == (uint32_t)RESET);
    }

    RTC->TAFCR &= (uint32_t)~RTC_TAFCR_ALARMOUTTYPE;
    RTC->TAFCR |= (uint32_t)(RTC_OUTPUT_TYPE_OPENDRAIN);

    /* Enable the write protection for RTC registers */
    RTC->WPR = 0xFFU;

    /* Clear all the alarm's list information to guarantee proper
     * initialization */
    for(currAlarm = 0; currAlarm < RTC_MAX_ALARMS; ++currAlarm)
    {
      ListOfAlarms[currAlarm].isRunning = FALSE;
    }

    /* Keep remembered that the RTC is running. This is mostly kept as in
     * order to avoid changing other parts of this driver. */
    RTC_ClockIsRunning = TRUE;

    /* Clear the RawTimeCount variables */
    RawTimeCount_CurrentValue = 0;
    RawTimeCount_LastSecondsValue = 0;

    /* Job done */
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/**
 * @brief  Initialize the internal RTC.
 * @param  DateTimePtr: Holds the acquired DateTime
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 * @note   This routine will return he current time available in the RTC.
 *
 */
ReturnCode_t RTC_GetDateTime(DateTime_t *DateTimePtr)
{
  TimeBDC_t TimeBCDTemp;
  DateBDC_t DateBCDTemp;
  uint32_t timetmpreg = 0U;
  uint32_t datetmpreg = 0U;

  /* Get the TR register */
  timetmpreg = (uint32_t)(RTC->TR & RTC_TR_RESERVED_MASK);

  /* Fill the structure fields with the read parameters */
  TimeBCDTemp.Hours = (uint8_t)((timetmpreg & (RTC_TR_HT | RTC_TR_HU)) >> 16U);
  TimeBCDTemp.Mins = (uint8_t)((timetmpreg & (RTC_TR_MNT | RTC_TR_MNU)) >> 8U);
  TimeBCDTemp.Secs = (uint8_t)(timetmpreg & (RTC_TR_ST | RTC_TR_SU));
  TimeBCDTemp.Padding = 0x00;

  /* Get the DR register */
  datetmpreg = (uint32_t)(RTC->DR & RTC_DR_RESERVED_MASK);

  /* Fill the structure fields with the read parameters */
  DateBCDTemp.Years = (uint8_t)((datetmpreg & (RTC_DR_YT | RTC_DR_YU)) >> 16U);
  DateBCDTemp.Years |= 0x2000;
  DateBCDTemp.Months = (uint8_t)((datetmpreg & (RTC_DR_MT | RTC_DR_MU)) >> 8U);
  DateBCDTemp.Days = (uint8_t)(datetmpreg & (RTC_DR_DT | RTC_DR_DU));

  /* Convert time and date from BCD format */
  Time_TimeFromBCD( &TimeBCDTemp, &DateTimePtr->TimeVal );
  Time_DateFromBCD( &DateBCDTemp, &DateTimePtr->DateVal );

  return ANSWERED_REQUEST;
}

/**
 * @brief  Initialize the internal RTC.
 * @param  DateTimePtr: New DateTime information
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: DateTime updated.
 *             @arg Else: Some error happened.
 * @note   This routine will set a new DateTime information to the RTC driver.
 *
 */
ReturnCode_t RTC_SetDateTime(DateTime_t *DateTimePtr)
{
  ReturnCode_t ReturnValue;
  TimeStamp_t Seconds;
  TimeStamp_t currTime;
  DateTime_t ActualTime;

  TimeBDC_t TimeFormatedBCD;
  DateBDC_t DateFormatedBCD;
  uint32_t timetmpreg = 0U;
  uint32_t datetmpreg = 0U;

  /* Test correctness of given parameters */
  if( RTC_EvaluateDateTime(DateTimePtr) == FALSE )
  { /* If given value is not correct then error */
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  { /* If the above checks didn't result in error, continue configuring the
     * peripheral.
     * Calculate the timestamp from the given date. */
    Time_ToTimeStamp(DateTimePtr, &Seconds);

    /* Before setting the new value, store the previous one, as it may be
     *  used in the alarm and RawTimeCount adjustment section. */
    RTC_GetDateTime(&ActualTime);
    Time_ToTimeStamp( &ActualTime, &currTime );

    /* With the new seconds value calculated and the current one at hands,
     * check if they are really different and, consequently, if time has to be
     * set.
     */
    if( currTime != Seconds )
    { /* Indeed, a new value has to be set. */
      /* If the new value is higher than the previous one, then the alarms have
       * to be checked to see if they should be triggered. */
      if( ( RTC_ClockIsRunning != FALSE ) && ( Seconds > currTime ) )
      { /* Do a procedure similar to the one that is performed in the interrupt
      routine. */
        uint32 CurrentValue;
        uint8  currId;

        /* Run through every alarm. If it is enabled and its value matches the
         * current alarm (or is lower than it), then trigger its flag and
         * disable it. */
        for(currId = 0; currId < RTC_MAX_ALARMS; ++currId)
        {
          if( ListOfAlarms[currId].isRunning != FALSE )
          {
            CurrentValue = ListOfAlarms[currId].TriggeringTime;

            if( CurrentValue <= Seconds )
            { /* Trigger this alarm. */
              ListOfAlarms[currId].AlarmFlag = TRUE;
              ListOfAlarms[currId].NumEventAlarmFlag++;
              ListOfAlarms[currId].isRunning = FALSE;
            }
          }
        }
      }

      /* Now, it is necessary to update the RawTimeCount value according to the
       * current time value, so that it keeps counting time correctly. */
      if( currTime > RawTimeCount_LastSecondsValue ) /* Sanity check */
      {
        RawTimeCount_CurrentValue += ( currTime - RawTimeCount_LastSecondsValue );
      }
      /* Set the RawTimeCount's last seconds value to the new time that is
       * being configured, so that it keeps working transparently. */
      RawTimeCount_LastSecondsValue = Seconds;

      Time_TimeToBCD(&DateTimePtr->TimeVal, &TimeFormatedBCD);
      Time_DateToBCD(&DateTimePtr->DateVal, &DateFormatedBCD);

      timetmpreg = (((uint32_t)(TimeFormatedBCD.Hours) << 16U) | \
                    ((uint32_t)(TimeFormatedBCD.Mins) << 8U) | \
                    ((uint32_t)TimeFormatedBCD.Secs));

      datetmpreg = ((((uint32_t)DateFormatedBCD.Years) << 16U) | \
                    (((uint32_t)DateFormatedBCD.Months) << 8U) | \
                    ((uint32_t)DateFormatedBCD.Days));

      /* Disable the write protection for RTC registers */
      RTC->WPR = 0xCAU;
      RTC->WPR = 0x53U;

      /* Check if the Initialization mode is set */
      if((RTC->ISR & RTC_ISR_INITF) == (uint32_t)RESET)
      {
        /* Set the Initialization mode */
        RTC->ISR = (uint32_t)RTC_INIT_MASK;
        /* Wait till RTC is in INIT state */
        while((RTC->ISR & RTC_ISR_INITF) == (uint32_t)RESET);
      }

      /* Set the RTC_TR register */
      RTC->TR = (uint32_t)(timetmpreg & RTC_TR_RESERVED_MASK);
      /* Set the RTC_DR register */
      RTC->DR = (uint32_t)(datetmpreg & RTC_DR_RESERVED_MASK);

      /* Exit Initialization mode */
      RTC->ISR &= (uint32_t)~RTC_ISR_INIT;

      //TODO Comment here
      if((RTC->CR & RTC_CR_BYPSHAD) == RESET)
      {
        /* Clear RSF flag */
        RTC->ISR &= (uint32_t)RTC_RSF_MASK;

        /* Wait the registers to be synchronised */
        while((RTC->ISR & RTC_ISR_RSF) == (uint32_t)RESET);
      }

      /* Enable the write protection for RTC registers */
      RTC->WPR = 0xFFU;
    }

    /* Flag that the RTC is running with a defined date and time */
    RTC_ClockIsRunning = TRUE;

    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/**
 * @brief  Get the internal RTC value in a TimeStamp format.
 * @param  TimeStamp: Holds the acquired TimeStamp
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg ERR_DISABLED: RTC not enabled
 *             @arg Else: Some error happened.
 * @note   This routine will read the RTC and will provide its DateTime
 *         information in a timestamp format.
 *
 */
ReturnCode_t RTC_GetTimeStamp(TimeStamp_t * TimeStamp)
{
  ReturnCode_t ReturnValue;
  DateTime_t ActualTime;
  if( RTC_ClockIsRunning != FALSE )
  {
    RTC_GetDateTime(&ActualTime);
    Time_ToTimeStamp( &ActualTime, TimeStamp );
    /* Simply return the time value contained in the peripheral's time count  */
    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* Clock is not yet running, flag error */
    *TimeStamp = 0;
    ReturnValue = ERR_DISABLED;
  }

  return ReturnValue;
}

/**
 * @brief  Provides the current RawTimeCount value.
 * @param  RawTimeCount: Holds the TimeStamp information requested
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg ERR_DISABLED: RTC not enabled
 *             @arg Else: Some error happened.
 * @note   The RawTimeCount is a time count variable that is incremented
 *         according to the real (according to the RTC peripheral) time pass.
 *         So, it is not affected by external time change events and therefore
 *         is a more reliable time information for libraries that depends of
 *         time passing information.
 *
 */
ReturnCode_t RTC_GetRawTimeCount(TimeStamp_t * RawTimeCount)
{
  ReturnCode_t ReturnValue;
  uint32 Rtc_CurrSecondsValue;
  TimeStamp_t RTC_TimeStamp;

  if( RTC_ClockIsRunning != FALSE )
  {
    /* This routine will return the RawTimeCount_CurrentValue data but, before
     * doing that, update its value according to the RTC information. */

    /* Get the current RTC value */
    RTC_GetTimeStamp(&RTC_TimeStamp);
    Rtc_CurrSecondsValue = RTC_TimeStamp;
    /* Perform the update */
    if( Rtc_CurrSecondsValue > RawTimeCount_LastSecondsValue ) /* Sanity check */
    {
      RawTimeCount_CurrentValue += ( Rtc_CurrSecondsValue - RawTimeCount_LastSecondsValue );
    }
    /* Update the RawTimeCount_LastSecondsValue variable */
    RawTimeCount_LastSecondsValue = Rtc_CurrSecondsValue;

    /* Done. Return the value RawTimeCount value */
    *RawTimeCount = RawTimeCount_CurrentValue;

    ReturnValue = ANSWERED_REQUEST;
  }
  else
  { /* RTC is not running. So, there is no RawTimeCount calculation */
    ReturnValue = ERR_DISABLED;
  }

  return ReturnValue;
}

/**
 * @brief  Start an alarm operation.
 * @param  ID: Alarm's ID
 * @param  DateTimePtr - DateTime value when the alarm should be triggered.
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg ERR_PARAM_ID: ID not valid
 *             @arg ERR_DISABLED: RTC not enabled
 *             @arg ERR_PARAM_RANGE: DateTime not valid
 *             @arg Else: Some error happened.
 * @note   This routine will start an alarm operation, which will trigger when
 *         the RTC's DateTime matches the one provided in the argument.
 *
 */
ReturnCode_t RTC_SetAlarmDateTime(uint8 ID, DateTime_t *DateTimePtr)
{
  ReturnCode_t ReturnValue;
  uint32 Seconds;

  /* Check if ID is valid. If it isn't, stop right away. */
  if( ID >= RTC_MAX_ALARMS )
  { /* ID out of range */
    ReturnValue = ERR_PARAM_ID;
  }
  else if( RTC_ClockIsRunning == FALSE )
  { /* RTC clock is not running */
    ReturnValue = ERR_DISABLED;
  } 
  else if( RTC_EvaluateDateTime(DateTimePtr) == FALSE )
  { /* If given value is not correct then error */
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  { /* Calculate the timestamp from the given DateTime */
    Time_ToTimeStamp( DateTimePtr, &Seconds );

    /* With the 'seconds' value validated and defined, call the alarm manager
     * routine to process this new value. */
    ListOfAlarms[ID].TriggeringTime = Seconds;
    ListOfAlarms[ID].isRunning = TRUE;
    ListOfAlarms[ID].AlarmFlag = FALSE;

    /* Job done */
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/**
 * @brief  Start an alarm operation, using Delta value, in seconds.
 * @param  ID: Alarm's ID
 * @param  TimeDelta - Time Delta value, in seconds, indicating when the alarm
 *         should be triggered, using the current DateTime as ref.
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg ERR_PARAM_ID: ID not valid
 *             @arg ERR_DISABLED: RTC not enabled
 *             @arg ERR_PARAM_RANGE: DateTime not valid
 *             @arg Else: Some error happened.
 * @note   This routine will start an alarm operation, which will trigger after
 *         a time delta value in the future, according to the current DateTime.
 *
 */
ReturnCode_t RTC_SetAlarmByDelta(uint8 ID, uint32 TimeDelta)
{
  ReturnCode_t ReturnValue;
  TimeStamp_t RTC_TimeStamp;
  uint32 Seconds;

  /* Check if ID is valid. If it isn't, stop right away. */
  if( ID >= RTC_MAX_ALARMS )
  { /* ID out of range */
    ReturnValue = ERR_PARAM_ID;
  }
  else if( RTC_ClockIsRunning == FALSE )
  { /* RTC clock is not running */
    ReturnValue = ERR_DISABLED;
  }
  else if( TimeDelta == 0)
  { /* Invalid time was given */
    ReturnValue = ERR_PARAM_RANGE;
  }
  else
  { /* Regular operation */
    /* Calculate the alarm time */
    RTC_GetTimeStamp(&RTC_TimeStamp);
    Seconds = TimeDelta + RTC_TimeStamp;
    /* With the 'seconds' value validated and defined, call the alarm manager
     * routine to process this new value. */
    ListOfAlarms[ID].TriggeringTime = Seconds;
    ListOfAlarms[ID].isRunning = TRUE;
    ListOfAlarms[ID].AlarmFlag = FALSE;
    /* Job done */
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/**
 * @brief  Start an alarm operation, using Delta value, in seconds.
 * @param  ID: Alarm's ID
 * @param  AlarmInfo - Will hold the info regarding the requested alarm.
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg ERR_PARAM_ID: ID not valid
 *             @arg Else: Some error happened.
 * @note   This routine will provide all the information related to the given
 *         alarm. With it, the requester can know if it has triggered, for
 *         example.
 *
 */
ReturnCode_t RTC_GetAlarmInfo(uint8 ID, AlarmList * AlarmInfo)
{
  ReturnCode_t ReturnValue;
  TimeStamp_t RTC_TimeStamp;

  /* Check if ID is valid. If it isn't, stop right away. */
  if( ID >= RTC_MAX_ALARMS )
  { /* ID out of range */
    ReturnValue = ERR_PARAM_ID;
  }
  else
  { /* Regular operation */
    if(( ListOfAlarms[ID].isRunning != FALSE ) && (ListOfAlarms[ID].AlarmFlag == FALSE))
    { /* Check if alarm has triggered */
      RTC_GetTimeStamp(&RTC_TimeStamp);
      if( ListOfAlarms[ID].TriggeringTime <= RTC_TimeStamp)
      { /* Alarm has triggered */
        ListOfAlarms[ID].AlarmFlag = TRUE;
        ListOfAlarms[ID].isRunning = FALSE;
        ListOfAlarms[ID].NumEventAlarmFlag++;
      }
    }
    AlarmInfo->isRunning         = ListOfAlarms[ID].isRunning;
    AlarmInfo->AlarmFlag         = ListOfAlarms[ID].AlarmFlag;
    AlarmInfo->NumEventAlarmFlag = ListOfAlarms[ID].NumEventAlarmFlag;
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/**
 * @brief  Clear Alarm flag.
 * @param  ID: Alarm's ID
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg ERR_PARAM_ID: ID not valid
 *             @arg Else: Some error happened.
 *
 */
ReturnCode_t RTC_ClearAlarmFlag(uint8 ID)
{
  /* This routine simply clears the alarm flag */
  ReturnCode_t ReturnValue;

  /* Check if ID is valid. If it isn't, stop right away. */
  if( ID >= RTC_MAX_ALARMS )
  { /* ID out of range */
    ReturnValue = ERR_PARAM_ID;
  }
  else
  { /* Regular operation */
    ListOfAlarms[ID].AlarmFlag = FALSE;
    ReturnValue = ANSWERED_REQUEST;
  }

  return ReturnValue;
}

/**
 * @brief  Get RTC Alarm interrupt counter.
 * @param  Count: Holds the alarm event count
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg ANSWERED_REQUEST: All ok.
 *             @arg Else: Some error happened.
 *
 */
ReturnCode_t RTC_GetTotalAlarmCount(uint32 * Count)
{
  /* RtcInterruptCount is not being incremented. Always return 0 */
  *Count = RtcInterruptCount;
  return ANSWERED_REQUEST;
}

/**
 * @brief  Check if the provided DateTime is valid.
 * @param  DateTimePtr: DateTime that should be evaluated.
 * @retval Result: Result of Operation.
 *         This parameter can be one of the following values:
 *             @arg TRUE: DateTime is valid.
 *             @arg FALSE: DateTime is not valid.
 *
 */
static bool RTC_EvaluateDateTime(DateTime_t *DateTimePtr)
{
  /*                       -- JAN FEB MAR APR MAY JUN JUL AUG SEP OCT NOV DEC   */
  const uint8 ULY[13] = {0U,31U,28U,31U,30U,31U,30U,31U,31U,30U,31U,30U,31U};   /* Non-leap-year */
  const uint8  LY[13] = {0U,31U,29U,31U,30U,31U,30U,31U,31U,30U,31U,30U,31U};   /* Leap-year */

  bool Result = TRUE;
  /* Some checks will be performed below. If any fails, the answer will be
   * changed to failure. */
  if ( (DateTimePtr->DateVal.Years  < 2000U) || (DateTimePtr->DateVal.Years  > 2099U) ||
       (DateTimePtr->DateVal.Months >   12U) || (DateTimePtr->DateVal.Months ==   0U) ||
       (DateTimePtr->DateVal.Days   >   31U) || (DateTimePtr->DateVal.Days   ==   0U)  )
  {
    Result = FALSE;
  }
  /* Is given year non-leap-one? */
  else if (DateTimePtr->DateVal.Years & 3U)
  {
    /* Does the obtained number of days exceed number of days in the appropriate
     * month & year? */
    if (ULY[DateTimePtr->DateVal.Months] < DateTimePtr->DateVal.Days)
    {
      Result = FALSE;
    }
  }
  /* Is given year leap-one? */
  else
  {
    /* Does the obtained number of days exceed number of days in the appropriate
     * month & year? */
    if (LY[DateTimePtr->DateVal.Months] < DateTimePtr->DateVal.Days)
    {
      Result = FALSE;
    }
  }

  return Result;
}

