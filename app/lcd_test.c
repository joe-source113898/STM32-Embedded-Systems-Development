#include "hel_lcd.h"
#include "stm32g0xx_hal.h"

LCD_HandleTypeDef hlcd_test;

void LCD_Test(void)
{
    /* System initialization code here (HAL_Init, SystemClock_Config, etc.) */
  
    /* Initialize peripherals (like SPI, GPIO, etc.) here */

    /* Initialize the LCD */
    if(HEL_LCD_Init(&hlcd_test) != HEL_OK)
    {
        // Handle the error (for example, by stopping in an infinite loop)
        while(1);
    }
  
    /* Set the LCD Backlight ON */
    HEL_LCD_Backlight(&hlcd_test, LCD_ON);

    /* Write a string to the LCD */
    HEL_LCD_String(&hlcd_test, "Hello, World!");

    /* Move the cursor to the second line */
    HEL_LCD_SetCursor(&hlcd_test, 1, 0);

    /* Write another string to the LCD */
    HEL_LCD_String(&hlcd_test, "Testing LCD!");

    /* Set contrast */
    HEL_LCD_Contrast(&hlcd_test, 8);  // middle contrast value

    while (1)
    {
        /* Main loop content. For example, you can toggle the LCD backlight every second */
        HEL_LCD_Backlight(&hlcd_test, LCD_TOGGLE);
        HAL_Delay(1000); // Delay for 1 second
    }
}
