/**
 * @file app_clock.c
 * @brief Application Clock Source File
 */

#include "app_bsp.h"
#include "app_clock.h"
#include <stdio.h>

#define PRESCALER_1 0x7F
#define PRESCALER_2 0xFF

/* Function prototypes */
/* static void Display_RTC_Data(RTC_TimeTypeDef *time, RTC_DateTypeDef *date, RTC_AlarmTypeDef *alarm); */

/* RTC-related structures and variables */
RTC_HandleTypeDef hrtc = {0};           /* RTC handler variable */
RTC_TimeTypeDef sTime = {0};           /* RTC time handler variable */
RTC_DateTypeDef sDate = {0};           /* RTC date handler variable */
RTC_AlarmTypeDef sAlarm = {0};         /* RTC alarm handler variable */

/* Add more includes as needed */

/* Application message structure */
APP_MsgTypeDef Msg;

/* Application message structure for CAN application */
APP_MsgTypeDef CANMsg;

/* Functions */
/**
 * @brief Initialize the RTC clock
 */
void Clock_Init(void)
{
   hrtc.Instance = RTC; /* Specify the RTC instance */
   hrtc.Init.HourFormat = RTC_HOURFORMAT_24; /* Use 24-hour format */
   hrtc.Init.AsynchPrediv = PRESCALER_1; /* Asynchronous prescaler value - for LSE: 127 */
   hrtc.Init.SynchPrediv = PRESCALER_2; /* Synchronous prescaler value - for LSE: 255 */
   hrtc.Init.OutPut = RTC_OUTPUT_DISABLE; /* RTC output setting */
   HAL_RTC_Init(&hrtc); /* Initialize the RTC */

   /* Set the time: 21:30:00 */
   sTime.Hours = 0x21;
   sTime.Minutes = 0x30;
   sTime.Seconds = 0x00;
   HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
   
   /* Set the date: Wednesday, August 16, 2023 */
   sDate.Date = 0x16;
   sDate.Month = RTC_MONTH_AUGUST;
   sDate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
   sDate.Year = 0x23;
   HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
}

/**
 * @brief Handle clock-related state transitions and tasks
 */
void Clock_Task(void)
{
    static Clock_States currentClockState = CLOCK_IDLE_STATE; /* Initialize the clock states variable */
    static uint32_t ticker; /* Variable to handle the ticks */

    ticker = HAL_GetTick(); /* Get the first lecture of transcurred time */

    switch (currentClockState)
    {
        case CLOCK_IDLE_STATE:
            /* Check if there's a message */
            if (Msg.msg != SERIAL_MSG_NONE)
            {
                currentClockState = CLOCK_VALID_DATA_STATE; /* Move to VALID_DATA_STATE */
            }
            if ((HAL_GetTick() - ticker) >= 1000)
            {
                ticker = HAL_GetTick(); /* Update the variable ticker */
                currentClockState = CLOCK_DISPLAY_DATA_STATE; /* Move to DISPLAY_DATA_STATE */
            }
            break;

        case CLOCK_VALID_DATA_STATE:
            if (Msg.msg == SERIAL_MSG_TIME)
            {
                currentClockState = CLOCK_UPDATE_TIME_STATE; /* Move to UPDATE_TIME_STATE */
            }
            else if (Msg.msg == SERIAL_MSG_DATE)
            {
                currentClockState = CLOCK_UPDATE_DATE_STATE; /* Move to UPDATE_DATE_STATE */
            }
            else if (Msg.msg == SERIAL_MSG_ALARM)
            {
                currentClockState = CLOCK_UPDATE_ALARM_STATE; /* Move to UPDATE_ALARM_STATE */
            }
            else
            {
                currentClockState = CLOCK_IDLE_STATE; /* Move to IDLE_CLOCK_STATE */
            }

            break;
        
        case CLOCK_UPDATE_TIME_STATE:
            sTime.Hours = Msg.tm.tm_hour;
            sTime.Minutes = Msg.tm.tm_min;
            sTime.Seconds = Msg.tm.tm_sec;
            HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

            currentClockState = CLOCK_DISPLAY_DATA_STATE; /* Move to DISPLAY_DATA_STATE */
            break;

        case CLOCK_UPDATE_DATE_STATE:
            sDate.Date = Msg.tm.tm_mday;
            sDate.Month = Msg.tm.tm_mon;
            sDate.WeekDay = Msg.tm.tm_wday;
            sDate.Year = Msg.tm.tm_year;
            HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

            currentClockState = CLOCK_DISPLAY_DATA_STATE; /* Move to DISPLAY_DATA_STATE */
            break;

        case CLOCK_UPDATE_ALARM_STATE:
            sAlarm.AlarmTime.Hours = Msg.tm.tm_hour;
            sAlarm.AlarmTime.Minutes = Msg.tm.tm_min;
            HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BIN);

            currentClockState = CLOCK_DISPLAY_DATA_STATE;
            break;

        case CLOCK_DISPLAY_DATA_STATE:
            CANMsg.msg = Msg.msg; /* Enabler for displaying the clock message */

            CANMsg.tm.tm_hour = Msg.tm.tm_hour;
            CANMsg.tm.tm_min = Msg.tm.tm_min;
            CANMsg.tm.tm_sec = Msg.tm.tm_sec;
            CANMsg.tm.tm_mday = Msg.tm.tm_mday;
            CANMsg.tm.tm_mon = Msg.tm.tm_mon;
            CANMsg.tm.tm_wday = Msg.tm.tm_wday;
            CANMsg.tm.tm_year = Msg.tm.tm_year;

            Msg.msg = SERIAL_MSG_NONE; /* Reset message indicator */
            
            currentClockState = CLOCK_IDLE_STATE; /* Move to IDLE_CLOCK_STATE */
            break;
        
        default:
            currentClockState = CLOCK_IDLE_STATE; /* Invalid state, return to IDLE_STATE */
            break;
    }
}

/**
 * @brief Display the time, date, and alarm data using Semihosting
 * @param time Pointer to the RTC time structure
 * @param date Pointer to the RTC date structure
 * @param alarm Pointer to the RTC alarm structure
 */
/*static void Display_RTC_Data(RTC_TimeTypeDef *time, RTC_DateTypeDef *date, RTC_AlarmTypeDef *alarm)
{
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);

    printf("Time: %u:%u:%u\n\r", time->Hours, time->Minutes, time->Seconds);
    printf("Date: %u/%u/%u\n\r", date->Date, date->Month, date->Year);
    printf("Alarm: %u:%u\n\r", alarm->AlarmTime.Hours, alarm->AlarmTime.Minutes);        
    printf("\n\r"); 
}*/
