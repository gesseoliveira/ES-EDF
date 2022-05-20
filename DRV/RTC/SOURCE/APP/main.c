/* Includes */
#include <stddef.h>
#include "sys_cfg_stm32f407.h"
#include "stm32f4xx.h"
#include "RTC.h"
#include "types.h"

/* Private macro */
/* Private variables */
/* Private function prototypes */
/* Private functions */

#define ID_0 0
#define ID_1 1
#define ID_2 2

static DateTime_t DateTime;
static TimeStamp_t TimeStamp;

AlarmList Alarm1;
AlarmList Alarm2;
AlarmList Alarm3;

int main(void)
{
  uint32_t ii = 0;

  Sys_Enable_Peripherals_Clock();
  Sys_Configure_Clock_168MHz();
  Sys_Enable_LSE();

  (void)RTC_Init();

  DateTime.DateVal.Days = 30;
  DateTime.DateVal.Months = 07;
  DateTime.DateVal.Years = 2019;
  DateTime.TimeVal.Hours = 10;
  DateTime.TimeVal.Mins = 14;
  DateTime.TimeVal.Secs = 00;

  (void)RTC_SetDateTime(&DateTime);

  DateTime.TimeVal.Secs = 35;
  (void)RTC_SetAlarmDateTime(ID_0,&DateTime); // Trigger after 5 seconds

  DateTime.TimeVal.Secs = 50;
  (void)RTC_SetAlarmDateTime(ID_1,&DateTime); // Trigger after 15 seconds

  DateTime.TimeVal.Mins = 59;
  (void)RTC_SetAlarmDateTime(ID_2,&DateTime); // Trigger after 1 minute
  // and 59 seconds

  while (1)
  {
    (void)RTC_GetDateTime(&DateTime);
//    RTC_GetRawTimeCount(&TimeStamp);

    // The best way to monitor these flags is using segger ozone! ;)
    RTC_GetAlarmInfo(ID_0, &Alarm1);
    RTC_GetAlarmInfo(ID_1, &Alarm2);
    RTC_GetAlarmInfo(ID_2, &Alarm3);
    RTC_GetTimeStamp(&TimeStamp);

    ii++;
  }

  return 0;
}
