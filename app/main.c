#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "hel_lcd.h"
#include "app_can.h"

/* Add more includes as needed */

/* Function to initialize monitor handles (for debugging) */
// extern void initialise_monitor_handles(void);

// SPI_HandleTypeDef SpiHandle; /* Structure to handle the SPI */

// LCD_HandleTypeDef hlcd; /* Structure to handle the LCD */


int main(void)
{
    /* Initialize debugging monitor */
    // initialise_monitor_handles();

    /* Initialize hardware abstraction layer */
    HAL_Init();

    /* Initialize serial communication */
    Serial_Init();

    /* Initialize clock functionality */
    Clock_Init();

    /* Initialize the display */
    // Display_Init();
    
    /* Add more initializations as needed */

    while (1)
    {
        /* Execute serial communication task */
        Serial_Task();

        /* Execute clock task */
        Clock_Task();

        /* Execute the display task */
        // Display_Task();

        /* Execute the CAN task */
        CAN_Task();

        /* Add and execute other tasks as needed */
    }
}
