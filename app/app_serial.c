/**
 * @file app_serial.c
 * @brief Application Serial Source File
 */

/* Add more includes as needed */
#include "app_bsp.h"
#include "app_serial.h"

#define NIBBLE_LSB_EXTRACTOR 0x0F
#define CAN_FILTER_ID 0x111
#define CAN_MESSAGE_ID 0x122
#define CAN_OK_MESSAGE_BYTE 0x55
#define CAN_ERROR_MESSAGE_BYTE 0xAA

/* Add more global variables, definitions, and/or prototypes as needed */
FDCAN_HandleTypeDef CANHandler;  /* Structure type variable for CAN initialization */
FDCAN_RxHeaderTypeDef CANRxHeader; /* CAN Rx header structure */
FDCAN_TxHeaderTypeDef CANTxHeader; /* CAN Tx header structure */
FDCAN_FilterTypeDef CANFilter;     /* CAN filter structure */

static uint8_t RxData[8] = {0}; /* Buffer to save the message to receive */

uint8_t message; /* Auxiliary flag to handle CAN message reception */

extern APP_MsgTypeDef Msg; /* Application message structure */

/* Functions */
/**
 * @brief Callback for CAN FIFO 0 message reception
 * @param hfdcan FDCAN handle
 * @param RxFifo0ITs FIFO 0 interrupt status
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    /* Retrieve Rx messages from RX FIFO 0 */
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &CANRxHeader, RxData);

    message = 1u; /* Set flag after the message has been received */
}

/**
 * @brief Handle the reception of single-frame CAN messages
 * @param data Pointer to received data
 * @param size Pointer to size of the payload
 * @return 1 if successful, 0 otherwise
 */
static uint8_t CanTp_SingleFrameRx(uint8_t *data, uint8_t *size)
{
    uint8_t nibbleMSB = (data[0] >> 4) & NIBBLE_LSB_EXTRACTOR; /* MSB */
    uint8_t nibbleLSB = data[0] & NIBBLE_LSB_EXTRACTOR; /* LSB */
    uint8_t returnValue = 0;
    size[0] = nibbleLSB;

    if ((nibbleMSB == 0) && ((nibbleLSB > 0) && (nibbleLSB <= 7)))
    {
        for (uint8_t i = 0; i < size[0]; i++)
        {
            data[i] = data[i + 1]; /* Unpacking */
        }

        returnValue = 1;
    }

    return returnValue;
}


/**
 * @brief Handle the transmission of single-frame CAN messages
 * @param data Pointer to data to be transmitted
 * @param size Size of the data to be transmitted
 */
static void CanTp_SingleFrameTx(uint8_t *data, uint8_t size)
{
    data[0] = (0 << 4) | (size & NIBBLE_LSB_EXTRACTOR); /* Packing */

    HAL_FDCAN_AddMessageToTxFifoQ(&CANHandler, &CANTxHeader, data);
}


/**
 * @brief Initialize the CAN bus
 */
void Serial_Init(void)
{
    /* FDCAN1 module to transmit up to 100Kbps and sample point of 75% */
    /* fCAN = fHSI / CANHandler.Init.ClockDivider / CANHandler.Init.NominalPrescaler */
    /* fCAN = 16MHz / 1 / 10 = 1.6MHz */
    /* Time quantas: */
    /* Ntq = fCAN / CANbaudrate */
    /* Ntq = 1.6MHz / 100Kbps = 16 */
    /* Sample point: */
    /* Sp = (CANHandler.Init.NominalTimeSeg1 + 1 / Ntq) * 100 */
    /* Sp = ((11 + 1) / 16) * 100 = 75% */
    CANHandler.Instance = FDCAN1;
    CANHandler.Init.Mode = FDCAN_MODE_NORMAL;                    /* CAN Classic mode */
    CANHandler.Init.FrameFormat = FDCAN_FRAME_CLASSIC;           /* Classic frame */
    CANHandler.Init.ClockDivider = FDCAN_CLOCK_DIV1;             /* No APB divider for FDCAN module */
    CANHandler.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;   /* Tx buffer in Fifo mode */
    CANHandler.Init.NominalPrescaler = 10;                       /* CAN clock divider by 10 */
    CANHandler.Init.NominalSyncJumpWidth = 1;                    /* SWJ of 1 */
    CANHandler.Init.NominalTimeSeg1 = 11;                        /* Phase time seg1 + prop seg */
    CANHandler.Init.NominalTimeSeg2 = 4;                         /* Phase time seg2 */
    HAL_FDCAN_Init(&CANHandler);

    /* Set option to transmit the messages */
    CANTxHeader.IdType = FDCAN_STANDARD_ID;  /* 11 bits CAN ID */
    CANTxHeader.FDFormat = FDCAN_CLASSIC_CAN; /* Classic CAN format up to 8 bytes */
    CANTxHeader.TxFrameType = FDCAN_DATA_FRAME; /* Type of frame data */
    CANTxHeader.Identifier = CAN_MESSAGE_ID; /* CAN message ID */
    CANTxHeader.DataLength = FDCAN_DLC_BYTES_8; /* 8 bytes to transmit */

    /* Configure reception filters to Rx FIFO 0, the filter will only accept ID 0x111 */
    CANFilter.IdType = FDCAN_STANDARD_ID;       /* 11 bits ID */
    CANFilter.FilterIndex = 0;
    CANFilter.FilterType = FDCAN_FILTER_MASK;
    CANFilter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* Filter on FIFO 0 */
    CANFilter.FilterID1 = CAN_FILTER_ID; /* Filter ID */

    /* Change FDCAN instance from initialization mode to normal mode */
    HAL_FDCAN_Start(&CANHandler);

    /* Enable reception interrupts when a message arrives on FIFO 0 */
    HAL_FDCAN_ActivateNotification(&CANHandler, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/**
 * @brief Handle the serial communication state machine
 */
void Serial_Task(void)
{
    static States currentState = IDLE_STATE; /* Variable of type States to handle the states in the state machines */

    static uint8_t messageType; /* Variable to handle the message type */
    static uint8_t size = 0;    /* Variable to handle the size of the payloads of the received messages */

    static uint8_t okMessage[8] = {0x00, CAN_OK_MESSAGE_BYTE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /* OK state message */
    static uint8_t errorMessage[8] = {0x00, CAN_ERROR_MESSAGE_BYTE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; /* ERROR state message */

    uint8_t hour, minutes, seconds, day, month, yearMSB, yearLSB = 0; /* Validation message variables */

    switch (currentState)
    {
    case IDLE_STATE:
        if (message == 1)
        {
            if (CanTp_SingleFrameRx(RxData, &size) == 1)
            {
                currentState = MESSAGE_STATE; /* Move to MESSAGE_STATE */
                message = 0;
            }
            else
            {
                currentState = ERROR_STATE; /* Move to ERROR_STATE */
            }

            message = 0;
        }
        break;

    case MESSAGE_STATE:
        messageType = RxData[0]; /* Extract the message type from the Rx buffer (byte 1) */

        if (messageType == SERIAL_MSG_TIME)
        {   
            Msg.msg = SERIAL_MSG_TIME; /* Set the message type in the message structure */
            currentState = TIME_STATE; /* Move to TIME state */
        }
        else if (messageType == SERIAL_MSG_DATE)
        {   
            Msg.msg = SERIAL_MSG_DATE; /* Set the message type in the message structure */
            currentState = DATE_STATE; /* Move to DATE state */
        }
        else if (messageType == SERIAL_MSG_ALARM)
        {
            Msg.msg = SERIAL_MSG_ALARM; /* Set the message type in the message structure */
            currentState = ALARM_STATE; /* Move to ALARM state */
        }
        else
        {
            currentState = ERROR_STATE; /* Move to ERROR state */
        }
        break;

    case TIME_STATE:
        hour = BCDtoDecimal(RxData[1]);     /* Extract the parameter 1 (byte 2) */
        minutes = BCDtoDecimal(RxData[2]);  /* Extract the parameter 2 (byte 3) */
        seconds = BCDtoDecimal(RxData[3]);  /* Extract the parameter 3 (byte 4) */

        if (Validate_Time(hour, minutes, seconds))
        {
            Msg.tm.tm_hour = hour;
            Msg.tm.tm_min = minutes;
            Msg.tm.tm_sec = seconds;

            currentState = OK_STATE; /* Move to OK state */
        }
        else
        {
            currentState = ERROR_STATE; /* Move to ERROR state */
        }
        
        break;

    case DATE_STATE:
        day = BCDtoDecimal(RxData[1]);      /* Extract the parameter 1 (byte 2) */
        month = BCDtoDecimal(RxData[2]);    /* Extract the parameter 2 (byte 3) */
        yearMSB = BCDtoDecimal(RxData[3]);  /* Extract the parameter 3 (byte 4) */
        yearLSB = BCDtoDecimal(RxData[4]);  /* Extract the parameter 4 (byte 5) */

        if (Validate_Date(day, month, yearMSB, yearLSB))
        {
            Msg.tm.tm_mday = day;
            Msg.tm.tm_mon = month;
            Msg.tm.tm_year = yearMSB * 100 + yearLSB;
            Msg.tm.tm_wday = WeekDay(day, month, Msg.tm.tm_year);

            currentState = OK_STATE; /* Move to OK state */
        }
        else
        {
            currentState = ERROR_STATE; /* Move to ERROR state */
        }
        
        break;

    case ALARM_STATE:
        hour = BCDtoDecimal(RxData[1]);    /* Extract the parameter 1 (byte 2) */
        minutes = BCDtoDecimal(RxData[2]); /* Extract the parameter 2 (byte 3) */

        if (Validate_Alarm(hour, minutes))
        {
            Msg.tm.tm_hour = hour;
            Msg.tm.tm_min = minutes;

            currentState = OK_STATE; /* Move to OK state */
        }
        else
        {
            currentState = ERROR_STATE; /* Move to ERROR state */
        }
        
        break;

    case OK_STATE:
        CanTp_SingleFrameTx(okMessage, size); /* Send message for OK state */
        
        currentState = IDLE_STATE;           /* Return to IDLE state */
        break;

    case ERROR_STATE:
        CanTp_SingleFrameTx(errorMessage, size); /* Send message for ERROR state */
        
        currentState = IDLE_STATE;              /* Return to IDLE state */
        break;

    default:
        currentState = IDLE_STATE;              /* Return to IDLE state */
        break;
    }
}

/* Add more auxiliary private functions as needed */

/**
 * @brief Converts a BCD (Binary-Coded Decimal) value to its decimal equivalent.
 *
 * @param bcdValue The BCD value to be converted.
 * @return Decimal representation of the input BCD value.
 */
uint8_t BCDtoDecimal(uint8_t bcdValue)
{
    uint8_t tens = (bcdValue >> 4) & NIBBLE_LSB_EXTRACTOR; /* Extract the tens digit from the BCD value */
    uint8_t units = bcdValue & NIBBLE_LSB_EXTRACTOR;       /* Extract the units digit from the BCD value */

    uint8_t decimalValue = (tens * 10) + units; /* Calculate the decimal value by multiplying the tens digit by 10 and adding the units digit */

    return decimalValue;
}

/**
 * @brief Validates a given time for correctness.
 *
 * @param hour Hour value.
 * @param minutes Minute value.
 * @param seconds Second value.
 * @return 1 if the time is valid, 0 otherwise.
 */
uint8_t Validate_Time(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
    /* Check if the hour, minutes, and seconds are within valid ranges */
    uint8_t isValid = (hour < 24 && minutes < 60 && seconds < 60) ? 1 : 0;

    return isValid;
}

/**
 * @brief Validates a given date for correctness.
 *
 * @param day Day value.
 * @param month Month value.
 * @param yearMSB Most significant byte of the year.
 * @param yearLSB Least significant byte of the year.
 * @return 1 if the date is valid, 0 otherwise.
 */
uint8_t Validate_Date(uint8_t day, uint8_t month, uint8_t yearMSB, uint8_t yearLSB)
{
    uint8_t status = 1;

    /* Combines the most significant byte and the least significant byte of the year to get the full year */
    uint16_t year = yearMSB * 100 + yearLSB;

    /* Array representing the number of days in each month (non-leap year) */
    uint8_t days_Month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Checks if the year is outside the 1901-2099 range and month is outside the 1-12 range. 
    If either condition is met, the function will return 0, indicating the date is invalid */
    if ((year < 1901 || year > 2099) && (month < 1 || month > 12))
    {
        status = 0;
    }

    /* Checks if the year is a leap year. A leap year is divisible by 4 but not divisible by 100, unless it's divisible by 400 */
    int leap_year = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);

    /* If the year is a leap year, update February's days to 29 */
    if (leap_year)
    {
        days_Month[1] = 29;
    }

    /* Checks if the day is outside the valid range for the given month. 
    If it is, the function will return 0, indicating the date is invalid */
    if (day < 1 || day > days_Month[month - 1])
    {
        status = 0;
    }
    
    return status;
}

/**
 * @brief Calculates the day of the week for a given date.
 *
 * @param day Day value.
 * @param month Month value.
 * @param year Year value.
 * @return Day of the week (e.g., 0 for Sunday, 1 for Monday, ...).
 */
uint8_t WeekDay(uint8_t day, uint8_t month, uint16_t year)
{
    /* Array representing the number of days in each month (non-leap year) */ 
    uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Array representing the days of the week */ 
    uint8_t weekdays[] = {0, 1, 2, 3, 4, 5, 6};

    /* Checks if the year is a leap year. A leap year is divisible by 4 but not divisible by 100, unless it's divisible by 400 */
    int leap_year = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);

    /* If the year is a leap year, update February's days to 29 */
    if (leap_year)
    {
        daysInMonth[1] = 29;
    }

    /* Variable to calculate the total number of days since January 1st, 0000 until the given date */
    uint32_t totalDays = 0;

    /* Adds the number of days for each year from year 0 until the specified year */
    for (uint16_t y = 0; y < year; y++)
    {
        totalDays += leap_year ? 366 : 365;
    }

    /* Adds the number of days for each month from January until the specified month */
    for (uint8_t m = 1; m < month; m++)
    {
        totalDays += daysInMonth[m - 1];
    }

    /* Adds the remaining days in the current month */
    totalDays += day - 1;

    /* Calculates the day of the week */
    uint8_t weekday = (totalDays % 7);

    return weekdays[weekday];
}

/**
 * @brief Validates a given alarm time for correctness.
 *
 * @param hour Hour value for the alarm.
 * @param minutes Minute value for the alarm.
 * @return 1 if the alarm time is valid, 0 otherwise.
 */
uint8_t Validate_Alarm(uint8_t hour, uint8_t minutes)
{
    uint8_t status = 1;

    if (hour < 0 || hour > 23 || minutes < 0 || minutes > 59)
    {
        status = 0;
    }
    
    return status;
}
