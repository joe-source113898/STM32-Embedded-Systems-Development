#ifndef __APP_SERIAL_H__
#define __APP_SERIAL_H__

#include <stdint.h>

/**
 * @file app_serial.h
 * @brief Provides utility and serial communication functions.
 *
 * This header provides a collection of utility functions to handle date-time 
 * validations and conversion operations, as well as functions for serial 
 * communication initialization and task management.
 */

/* Function prototypes */

/**
 * @brief Converts a BCD (Binary-Coded Decimal) value to its decimal equivalent.
 *
 * @param bcdValue The BCD value to be converted.
 * @return Decimal representation of the input BCD value.
 */
uint8_t BCDtoDecimal(uint8_t bcdValue);

/**
 * @brief Validates a given time for correctness.
 *
 * @param hour Hour value.
 * @param minutes Minute value.
 * @param seconds Second value.
 * @return 1 if the time is valid, 0 otherwise.
 */
uint8_t Validate_Time(uint8_t hour, uint8_t minutes, uint8_t seconds);

/**
 * @brief Validates a given date for correctness.
 *
 * @param day Day value.
 * @param month Month value.
 * @param yearMSB Most significant byte of the year.
 * @param yearLSB Least significant byte of the year.
 * @return 1 if the date is valid, 0 otherwise.
 */
uint8_t Validate_Date(uint8_t day, uint8_t month, uint8_t yearMSB, uint8_t yearLSB);

/**
 * @brief Validates a given alarm time for correctness.
 *
 * @param hour Hour value for the alarm.
 * @param minutes Minute value for the alarm.
 * @return 1 if the alarm time is valid, 0 otherwise.
 */
uint8_t Validate_Alarm(uint8_t hour,uint8_t minutes);

/**
 * @brief Calculates the day of the week for a given date.
 *
 * @param day Day value.
 * @param month Month value.
 * @param year Year value.
 * @return Day of the week (e.g., 0 for Sunday, 1 for Monday, ...).
 */
uint8_t WeekDay(uint8_t day, uint8_t month, uint16_t year);

/**
 * @brief Initializes the serial communication module.
 *
 * This function prepares the serial communication module for operation. 
 * It should be called before any other serial communication functions.
 */
void Serial_Init(void);

/**
 * @brief Periodic serial communication task function.
 *
 * This function should be called periodically in the main application loop
 * or a dedicated task to maintain and manage serial communication functionalities.
 */
void Serial_Task(void);

#endif // __APP_SERIAL_H__
