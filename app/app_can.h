/**
 * @file app_can.h
 * @brief This file contains the declarations for CAN initialization and task handling.
 */

#ifndef __APP_CAN_H__
#define __APP_CAN_H__

/**
 * @brief Executes CAN tasks based on the current CAN state.
 * 
 * The function processes any pending CAN tasks by switching through different
 * CAN states. Depending on the current state, it prepares data for transmission,
 * updates the message identifier, and sends CAN messages.
 */
void CAN_Task(void);

#endif // __APP_CAN_H__
