/**
 * @file app_display.c
 * @brief Functions for initializing and managing the LCD display.
 */

#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "hel_lcd.h"
#include "app_display.h"
#include <stdio.h>

#define CLOCK_MESSAGE_ENABLED 1
#define CLOCK_MESSAGE_DISABLED 0

/* Function prototypes */


/* Add more includes as needed */
/* LCD handle structure definition */
extern LCD_HandleTypeDef hlcd;

/* Structure to handle the SPI interface */
extern SPI_HandleTypeDef SpiHandle;

/* Application clock message structure */
extern APP_MsgTypeDef ClockMsg;


/* Functions */
/**
 * @brief Initializes components required for LCD display.
 */
void Display_Init(void)
{   
     /* Structure to hold GPIO configuration details */
    GPIO_InitTypeDef GPIO_InitStruct;
    
    /* Enable the clocks for respective GPIO ports */
    __GPIOC_CLK_ENABLE(); 
    __GPIOB_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    /* Set up GPIO configuration parameters */
    GPIO_InitStruct.Pin = 0xFF;                  /* All pins */
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;  /* Push-pull mode */
    GPIO_InitStruct.Pull = GPIO_NOPULL;          /* No pull-up or pull-down */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; /* Low frequency */
    
    /* Apply GPIO configuration */
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* Set up SPI configuration parameters */
    SpiHandle.Instance = SPI1;
    SpiHandle.Init.Mode = SPI_MODE_MASTER;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    SpiHandle.Init.Direction = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SpiHandle.Init.DataSize = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS = SPI_NSS_SOFT;
    SpiHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    SpiHandle.Init.TIMode = SPI_TIMODE_DISABLED;

    /* Initialize the SPI with the specified parameters */
    HAL_SPI_Init(&SpiHandle);

    /* Configure LCD pins and SPI handler */
    hlcd.BklPin= LCD_BL_PIN;
    hlcd.BklPort = LCD_BL_PORT;
    hlcd.CsPin = LCD_CS_PIN; 
    hlcd.CsPort = LCD_CS_PORT;
    hlcd.RstPin = LCD_RST_PIN;
    hlcd.RstPort = LCD_RST_PORT;
    hlcd.RsPin = LCD_RS_PIN;
    hlcd.RsPort = LCD_RS_PORT;
    hlcd.SpiHandler = &SpiHandle;

    /* Set the LCD contrast */
    HEL_LCD_Contrast(&hlcd, 0x00);

    /* Initialize the LCD. If not successful, set GPIOC pin 0 */
    if (HEL_LCD_Init(&hlcd) != HEL_OK)
    {
        HAL_GPIO_WritePin(GPIOC, 0, GPIO_PIN_SET);
    }
}

/**
 * @brief Implements the state machine for message processing from the clock task and display on the LCD.
 */
void Display_Task(void)
{   
    static Display_States currentDisplayState = DISPLAY_IDLE_STATE; /* Set the IDLE state for the display state variable */
    static Time time = {0, 0, 0}; /* Define a time structure */
    static Date date = {0, 0, 0, 0}; /* Define a date structure */

    HEL_LCD_SetCursor(&hlcd, 0, 0);

    HEL_LCD_String(&hlcd, "H");

    switch(currentDisplayState) 
    {
        case DISPLAY_IDLE_STATE:
            HEL_LCD_SetCursor(&hlcd, 0, 0);

            HEL_LCD_String(&hlcd, "H");

            /* Wait/check for a message from the Clock_Task() function */
            if (ClockMsg.msg == CLOCK_MESSAGE_ENABLED) 
            {
                currentDisplayState = DISPLAY_PROCESS_MESSAGE_STATE; /* Move to DISPLAY_PROCESS_MESSAGE_STATE */
            }
            
            break;

        case DISPLAY_PROCESS_MESSAGE_STATE:
            /* Parse the ClockMsg to extract time and date */
            
            /* Fill out the time structure */
            time.hour = ClockMsg.tm.tm_hour;
            time.min = ClockMsg.tm.tm_min;
            time.sec = ClockMsg.tm.tm_sec;
            
            /* Fill out the date structure */
            date.day = ClockMsg.tm.tm_mday;
            date.month = ClockMsg.tm.tm_mon;
            date.year = ClockMsg.tm.tm_year;
            date.weekday = ClockMsg.tm.tm_wday;

            currentDisplayState = DISPLAY_UPDATE_DISPLAY_STATE; /* Move to DISPLAY_UPDATE_DISPLAY_STATE */

            break;

        case DISPLAY_UPDATE_DISPLAY_STATE:
            /* Update the LCD with the time and date */
            data_string(date.weekday, date.month, date.day, date.year);
            time_string(time.hour, time.min, time.sec);
            
            currentDisplayState = DISPLAY_CLEAR_MESSAGE_STATE; /* Move to DISPLAY_CLEAR_MESSAGE_STATE */

            break;

        case DISPLAY_CLEAR_MESSAGE_STATE:
            ClockMsg.msg = CLOCK_MESSAGE_DISABLED; /* Clear the ClockMsg variable */
            currentDisplayState = DISPLAY_IDLE_STATE; /* Move to DISPLAY_IDLE_STATE */

            break;

        default:
            /* Handle unexpected states (optional) */
            currentDisplayState = DISPLAY_IDLE_STATE; /* Move to DISPLAY_IDLE_STATE */

            break;
    }
}

void data_string(uint8_t wday, uint8_t month, uint8_t mday, uint32_t year)
{
    char date[15];
    const char *monthNames[] = {"Ene", "Feb", "Mar", "Abr", "May", "Jun", "Jul", "Ago", "Sep", "Oct", "Nov", "Dic"};
    const char *monthName = monthNames[month - (uint8_t)1];

    for (int i = 0; i < 3; i++)
    {
        date[i] = monthName[i];
    }

    date[4] = ((mday / (uint8_t)10) % (uint8_t)10) + (uint8_t)48;
    date[5] = (mday % (uint8_t)10) + (uint8_t)48;
    date[6] = 32;
    date[7] = (year / (uint32_t)1000) + '0';
    date[8] = (year / (uint32_t)100) % (uint32_t)10 + '0';
    date[9] = ((year / (uint32_t)10) % (uint32_t)10) + (uint32_t)48;
    date[10] = (year % (uint32_t)10) + (uint32_t)48;
    date[11] = 32;

    const char *dayOfWeekNames[] = {"Do", "Lu", "Ma", "Mi", "Ju", "Vi", "Sa"};
    const char *dayOfWeekName = dayOfWeekNames[wday - (uint8_t)1];

    for (int i = 0; i < 2; i++)
    {
        date[i + 12] = dayOfWeekName[i];
    }
    date[14] = '\0';

    HEL_LCD_SetCursor(&hlcd, 0, 1);
    
    HEL_LCD_String(&hlcd, date);
}

void time_string(uint32_t hours, uint32_t minutes, uint32_t seconds)
{
    char time[9];
    time[0] = (hours / (uint32_t)10) + (uint32_t)48;
    time[1] = (hours % (uint32_t)10) + (uint32_t)48;
    time[2] = 58;
    time[3] = (minutes / (uint32_t)10) + (uint32_t)48;
    time[4] = (minutes % (uint32_t)10) + (uint32_t)48;
    time[5] = 58;
    time[6] = (seconds / (uint32_t)10) + (uint32_t)48;
    time[7] = (seconds % (uint32_t)10) + (uint32_t)48;
    time[8] = '\0';

    HEL_LCD_SetCursor(&hlcd, 1, 3);

    HEL_LCD_String(&hlcd, time);
}
