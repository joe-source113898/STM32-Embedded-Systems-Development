#ifndef __APP_CLOCK_H__
#define __APP_CLOCK_H__

#include <stdint.h>

/**
 * @file app_clock.h
 * @brief Provides initialization and task functions for a clock application.
 *
 * This file declares the primary functions required to interface with the
 * clock application. It includes initialization and task functions to be 
 * used by other modules or the main application.
 */

/* Function prototypes */

/**
 * @brief Initializes the clock module.
 *
 * This function prepares the clock application for operation. It must be called 
 * before any other clock functions.
 */
void Clock_Init(void);

/**
 * @brief Periodic clock task function.
 *
 * This function should be called periodically in the main application loop
 * or a dedicated task to maintain and update the clock functionalities.
 */
void Clock_Task(void);

#endif // __APP_CLOCK_H__
