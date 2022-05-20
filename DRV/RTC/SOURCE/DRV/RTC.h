/**
 *******************************************************************************
 * @file    RTC.h
 * @author  Peterson Aguiar
 * @brief   RTC Internal module driver.
 * @brief   Version 2019.9.1
 * @brief   Creation Date 23/07/2019
 *******************************************************************************
 */

#ifndef RTC_H_INCLUDED
#define RTC_H_INCLUDED

#include "returncode.h"
#include "SETUP.h"
#include "types.h"

#define DRV_RTC_VER_MAJOR      2019
#define DRV_RTC_VER_MINOR      9
#define DRV_RTC_VER_PATCH      1
#define RTC_BRANCH_MASTER      1

/**
 * Max number of Alarms.
 */
#ifndef RTC_MAX_ALARMS
#define RTC_MAX_ALARMS 10
#endif

typedef struct {
  bool   isRunning;
  bool   AlarmFlag;
  uint32 NumEventAlarmFlag;
} AlarmList;

typedef struct
{
  uint32 TriggeringTime;        /* Stores the time value, in timestamp format, that this alarm will trigger */
  uint32 NumEventAlarmFlag;     /* How many times this alarm has already been triggered */
  bool   isRunning;             /* Flags if this alarm is running (and should be considered) or not */
  bool   AlarmFlag;             /* Flags if this alarm has expired or not */
} AlarmArray_t;

ReturnCode_t RTC_Init(void);
ReturnCode_t RTC_GetDateTime(DateTime_t *DateTimePtr);
ReturnCode_t RTC_SetDateTime(DateTime_t *DateTimePtr);
ReturnCode_t RTC_SetAlarmDateTime(uint8 ID, DateTime_t *DateTimePtr);
ReturnCode_t RTC_SetAlarmByDelta(uint8 ID, uint32 TimeDelta);
ReturnCode_t RTC_GetAlarmInfo(uint8 ID, AlarmList * AlarmInfo);
ReturnCode_t RTC_ClearAlarmFlag(uint8 ID);
ReturnCode_t RTC_GetTotalAlarmCount(uint32 * Count);
ReturnCode_t RTC_GetTimeStamp(TimeStamp_t * TimeStamp);
ReturnCode_t RTC_GetRawTimeCount(TimeStamp_t * RawTimeCount);

#endif 
