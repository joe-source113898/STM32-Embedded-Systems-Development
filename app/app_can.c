/**
 * @file app_can.c
 * @brief This file contains the definitions for CAN initialization and task handling.
 */

#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "app_can.h"
#include <stdio.h>

#define CAN_TIME_MESSAGE_ID 0x130
#define CAN_DATE_MESSAGE_ID 0x131
#define CAN_ALARM_MESSAGE_ID 0x132

/* External Variables, Definitions, and Prototypes */
extern FDCAN_HandleTypeDef CANHandler;  /* Structure type variable for CAN initialization */
extern FDCAN_TxHeaderTypeDef CANTxHeader; /* CAN Tx header structure */
extern FDCAN_FilterTypeDef CANFilter;     /* CAN filter structure */

extern APP_MsgTypeDef CANMsg; /* Application message structure for CAN application */

/**
 * @brief Executes CAN tasks based on the current CAN state.
 * 
 * The function processes any pending CAN tasks by switching through different
 * CAN states. Depending on the current state, it prepares data for transmission,
 * updates the message identifier, and sends CAN messages.
 */
void CAN_Task(void)
{
    /* Static buffer to hold the message data to be sent */
    static uint8_t TxData[8] = {0};      

    /* Static variable to hold the current state of CAN operations */
    static CAN_StateTypeDef currentCanState = CAN_IDLE_STATE;

    switch(currentCanState)
    {
        /* Check the state and process accordingly */
        
        case CAN_IDLE_STATE:
            /* If a message is pending, update the state to prepare for transmission */
            if(CANMsg.msg == 1)
            {
                currentCanState = CAN_SEND_TIME_STATE;
            }
            else if(CANMsg.msg == 2)
            {
                currentCanState = CAN_SEND_DATE_STATE;
            }
            else if(CANMsg.msg == 3)
            {
                currentCanState = CAN_SEND_ALARM_STATE;
            }
            break;

        case CAN_SEND_TIME_STATE:
            /* Prepare time data for transmission */
            TxData[0] = (uint8_t) CANMsg.tm.tm_hour;
            TxData[1] = (uint8_t) CANMsg.tm.tm_min;
            TxData[2] = (uint8_t) CANMsg.tm.tm_sec;

            /* Set message identifier and transmit time data using FDCAN */
            CANTxHeader.Identifier = CAN_TIME_MESSAGE_ID; /* CAN time message ID */
            HAL_FDCAN_AddMessageToTxFifoQ(&CANHandler, &CANTxHeader, TxData);

            /* Reset message indicator and revert to idle state */
            CANMsg.msg = 0;
            currentCanState = CAN_IDLE_STATE;
            break;

        case CAN_SEND_DATE_STATE:
            /* Prepare date data for transmission */
            TxData[0] = (uint8_t) CANMsg.tm.tm_mday;
            TxData[1] = (uint8_t) CANMsg.tm.tm_mon;
            TxData[2] = (uint8_t) (CANMsg.tm.tm_year / 100); /* Most significant 2 digits of year */
            TxData[3] = (uint8_t) (CANMsg.tm.tm_year % 100); /* Least significant 2 digits of year */
            
            /* Set message identifier and transmit date data using FDCAN */
            CANTxHeader.Identifier = CAN_DATE_MESSAGE_ID; /* CAN date message ID */
            HAL_FDCAN_AddMessageToTxFifoQ(&CANHandler, &CANTxHeader, TxData);
            
            /* Reset message indicator and revert to idle state */
            CANMsg.msg = 0;
            currentCanState = CAN_IDLE_STATE;
            break;

        case CAN_SEND_ALARM_STATE:
            /* Prepare alarm data for transmission */
            TxData[0] = (uint8_t) CANMsg.tm.tm_hour;
            TxData[1] = (uint8_t) CANMsg.tm.tm_min;

            /* Set message identifier and transmit alarm data using FDCAN */
            CANTxHeader.Identifier = CAN_ALARM_MESSAGE_ID; /* CAN alarm message ID */
            HAL_FDCAN_AddMessageToTxFifoQ(&CANHandler, &CANTxHeader, TxData);
            
            /* Reset message indicator and revert to idle state */
            CANMsg.msg = 0;
            currentCanState = CAN_IDLE_STATE;
            break;

        default:
            /* In case of an unknown state, revert to idle state */
            currentCanState = CAN_IDLE_STATE;
        break;
    }
}