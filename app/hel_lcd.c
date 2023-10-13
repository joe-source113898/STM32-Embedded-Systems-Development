/**
 * @file    hel_lcd.c
 * @brief   Functions for initializing and controlling an LCD display.
 */

#include "hel_lcd.h"
#include "stm32g0xx_hal.h"

/**
 * @brief  Initialize the LCD display.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @retval Status: HEL_OK if initialization succeeds, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Init(LCD_HandleTypeDef *hlcd) 
{
    uint8_t status = HEL_OK;

    /* Ensure that the pointer is not NULL */
    if(hlcd == NULL || hlcd->SpiHandler == NULL)
    {
        status = HEL_ERROR;
    }

    /* Initialize the LCD with platform-specific settings */
    HEL_LCD_MspInit(hlcd);

    /* Begin LCD RESET sequence */
    /* Set the LCD backlight pin to a high state */
    HAL_GPIO_WritePin(hlcd->BklPort, hlcd->BklPin, GPIO_PIN_SET);
    /* Set the LCD chip select pin to a high state */
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, GPIO_PIN_SET);
    /* Set the LCD reset pin to a low state */
    HAL_GPIO_WritePin(hlcd->RstPort, hlcd->RstPin, GPIO_PIN_RESET);
    /* Wait for 2 ms before changing the reset pin state */
    HAL_Delay(2);
    /* Set the LCD reset pin to a high state, ending the reset sequence */
    HAL_GPIO_WritePin(hlcd->RstPort, hlcd->RstPin, GPIO_PIN_SET);
    /* Wait for 20 ms to ensure the LCD is ready after the reset */
    HAL_Delay(20);

    /* Send initialization commands to the LCD */
    if (HEL_LCD_Command(hlcd, 0x30) != HEL_OK) status = HEL_ERROR; /* Wake up command */

    HAL_Delay(2); /* Wait for 2 ms between commands */
    
    if (HEL_LCD_Command(hlcd, 0x30) != HEL_OK) status = HEL_ERROR; /* Another wake up command */
    if (HEL_LCD_Command(hlcd, 0x30) != HEL_OK) status = HEL_ERROR; /* Yet another wake up command */
    if (HEL_LCD_Command(hlcd, 0x39) != HEL_OK) status = HEL_ERROR; /* Function set command */
    if (HEL_LCD_Command(hlcd, 0x14) != HEL_OK) status = HEL_ERROR; /* Set internal oscillator frequency */
    if (HEL_LCD_Command(hlcd, 0x56) != HEL_OK) status = HEL_ERROR; /* Power control command */
    if (HEL_LCD_Command(hlcd, 0x6D) != HEL_OK) status = HEL_ERROR; /* Follower control command */
    if (HEL_LCD_Command(hlcd, 0x70) != HEL_OK) status = HEL_ERROR; /* Set contrast command */
    if (HEL_LCD_Command(hlcd, 0x0C) != HEL_OK) status = HEL_ERROR; /* Display on command */
    if (HEL_LCD_Command(hlcd, 0x06) != HEL_OK) status = HEL_ERROR; /* Set entry mode */
    if (HEL_LCD_Command(hlcd, 0x01) != HEL_OK) status = HEL_ERROR; /* Clear display command */

    /* Wait for 10 ms to ensure commands are processed by the LCD */
    HAL_Delay(10);

    return status;
}

/**
 * @brief  Initialize the LCD display GPIO/SPI as needed.
 * @param  hlcd: Pointer to the LCD handle structure.
 */
__weak void HEL_LCD_MspInit(LCD_HandleTypeDef *hlcd) 
{
    /* User-specific GPIO/SPI initialization if needed */
}

/**
 * @brief  Send a command to the LCD display.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @param  cmd: The command to send.
 * @retval Status: HEL_OK if the command is sent successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Command(LCD_HandleTypeDef *hlcd, uint8_t cmd) 
{
    uint8_t status = HEL_OK;

    HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(hlcd->SpiHandler, &cmd, 1, 5000);

    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, GPIO_PIN_SET);

    return status;
}

/**
 * @brief  Send data to the LCD display.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @param  data: The data to send.
 * @retval Status: HEL_OK if data is sent successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Data(LCD_HandleTypeDef *hlcd, uint8_t data) 
{
    uint8_t status = HEL_OK;

    HAL_GPIO_WritePin(hlcd->RsPort, hlcd->RsPin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(hlcd->SpiHandler, &data, 1, 5000);

    HAL_GPIO_WritePin(hlcd->CsPort, hlcd->CsPin, GPIO_PIN_SET);

    HAL_Delay(2); /* Wait for 2 ms between commands */

    return status;
}

/**
 * @brief  Display a string on the LCD.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @param  str: Pointer to the string to display.
 * @retval Status: HEL_OK if the string is displayed successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_String(LCD_HandleTypeDef *hlcd, char *str) 
{
    uint8_t status = HEL_OK;
    char *ptr = str;

    while (*ptr != '\0')
    {
        status = HEL_LCD_Data(hlcd, *ptr);
        ptr++;
    }

    return status;
}

/**
 * @brief  Set the cursor position on the LCD.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @param  row: Row number (0 or 1).
 * @param  col: Column number (0 to 15).
 * @retval Status: HEL_OK if the cursor position is set successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_SetCursor(LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col)
{
    uint8_t status = HEL_OK;

    if ((row > 1) || (col > 15))
    {
        status = HEL_ERROR;
    }

    uint8_t address = (row * 0x40) + col;

    status = HEL_LCD_Command(hlcd, 0x80 | address);

    return status;
}

/**
 * @brief  Control the LCD backlight.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @param  state: LCD_ON to turn on backlight, LCD_OFF to turn it off, LCD_TOGGLE to toggle it.
 */
void HEL_LCD_Backlight(LCD_HandleTypeDef *hlcd, uint8_t state) 
{
    if (state == LCD_ON)
    {
        HAL_GPIO_WritePin(hlcd->BklPort, hlcd->BklPin, GPIO_PIN_SET);
    }
    else if (state == LCD_OFF)
    {
        HAL_GPIO_WritePin(hlcd->BklPort, hlcd->BklPin, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_TogglePin(hlcd->BklPort, hlcd->BklPin);
    }
}

/**
 * @brief  Adjust the LCD contrast.
 * @param  hlcd: Pointer to the LCD handle structure.
 * @param  contrast: The contrast value (0 to 15).
 * @retval Status: HEL_OK if the contrast is adjusted successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Contrast(LCD_HandleTypeDef *hlcd, uint8_t contrast) 
{
    uint8_t status = HEL_OK;

    if (contrast < 15)
    {
        status = HEL_LCD_Command(hlcd, 0x70 | (contrast & 0x0F));
    }
    else
    {
        status = HEL_ERROR;
    }
    
    return status;
}
