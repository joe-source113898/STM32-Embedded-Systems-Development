#ifndef __APP_BSP_H__
#define __APP_BSP_H__

#include "stm32g0xx.h"
#include <stdint.h>

/**
 * @file app_bsp.h
 * @brief Application Board Support Package Header File
 */

/**
 * @brief Enum for the main state machines (Part I: State Machines)
 */
typedef enum
{
    IDLE_STATE,      /**< Idle state */
    MESSAGE_STATE,   /**< Message state */
    TIME_STATE,      /**< Time state */
    DATE_STATE,      /**< Date state */
    ALARM_STATE,     /**< Alarm state */
    OK_STATE,        /**< OK state */
    ERROR_STATE      /**< Error state */
} States;

/**
 * @brief Enum for application-specific message types (Part I: State Machines)
 */
typedef enum
{
    SERIAL_MSG_NONE = 0, /**< No message */
    SERIAL_MSG_TIME,     /**< Time message */
    SERIAL_MSG_DATE,     /**< Date message */
    SERIAL_MSG_ALARM     /**< Alarm message */
} APP_Messages;

/**
 * @brief Structure defining the time in a standardized format (Part I: State Machines)
 */
typedef struct _APP_TmTypeDef
{
    uint32_t tm_sec;    /**< Seconds, range 0 to 59 */
    uint32_t tm_min;    /**< Minutes, range 0 to 59 */
    uint32_t tm_hour;   /**< Hours, range 0 to 23 */
    uint32_t tm_mday;   /**< Day of the month, range 1 to 31 */
    uint32_t tm_mon;    /**< Month, range 0 to 11 */
    uint32_t tm_year;   /**< Years in range 1900 to 2100 */
    uint32_t tm_wday;   /**< Day of the week, range 0 to 6 */
    uint32_t tm_yday;   /**< Day in the year, range 0 to 365 */
    uint32_t tm_isdst;  /**< Daylight saving time */
} APP_TmTypeDef;

/**
 * @brief Structure defining the application message (Part I: State Machines)
 */
typedef struct _APP_MsgTypeDef
{
    uint8_t msg;           /**< Store the message type to send */
    APP_TmTypeDef tm;      /**< Time and date in stdlib tm format */
} APP_MsgTypeDef;

/**
 * @brief Enum for the state machine related to the RTC (Part II: Clock)
 */
typedef enum {
    CLOCK_IDLE_STATE,           /**< Idle state for clock */
    CLOCK_VALID_DATA_STATE,     /**< Valid data state for clock */
    CLOCK_UPDATE_TIME_STATE,    /**< Update time state for clock */
    CLOCK_UPDATE_DATE_STATE,    /**< Update date state for clock */
    CLOCK_UPDATE_ALARM_STATE,   /**< Update alarm state for clock */
    CLOCK_DISPLAY_DATA_STATE    /**< Display data state for clock */
} Clock_States;

/**
 * @brief Display handler structure (Part VII: Another Task to Control)
 *
 * Define the machine states of the display
 */
typedef enum {
    DISPLAY_IDLE_STATE, /*<< State: Idle */
    DISPLAY_PROCESS_MESSAGE_STATE, /*<< State: Process message */
    DISPLAY_UPDATE_DISPLAY_STATE, /*<< State: Update display */
    DISPLAY_CLEAR_MESSAGE_STATE /*<< State: Clear message */
} Display_States;

/**
 * @brief Structure representing a specific time of day (Part VII: Another Task to Control)
 */
typedef struct {
    uint32_t hour;   /**< Hours, range 0 to 23 */
    uint32_t min;    /**< Minutes, range 0 to 59 */
    uint32_t sec;    /**< Seconds, range 0 to 59 */
} Time;

/**
 * @brief Structure representing a specific date (Part VII: Another Task to Control)
 */
typedef struct {
    uint32_t day;      /**< Day of the month, range 1 to 31 */
    uint32_t month;    /**< Month, range 0 to 11 (or 1 to 12, depending on your preference) */
    uint32_t year;     /**< Year, e.g., 2023 */
    uint32_t weekday;  /**< Day of the week, range 0 (Sunday) to 6 (Saturday) */
} Date;

/**
 * @brief Structure for send a message through CAN (Part VII: Another Task to Control replacing SPI/LCD)
*/
typedef enum
{
    CAN_IDLE_STATE,
    CAN_SEND_TIME_STATE,
    CAN_SEND_DATE_STATE,
    CAN_SEND_ALARM_STATE
} CAN_StateTypeDef;

#endif /* __APP_BSP_H__ */
