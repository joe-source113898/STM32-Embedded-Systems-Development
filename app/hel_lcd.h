#ifndef __HEL_LCD_H__
#define __HEL_LCD_H__

#include "stm32g0xx.h"
#include <stdint.h>

/**
* @brief Header file for handling the LCD display.
*/

#define LCD_ON      0x01
#define LCD_OFF     0x00
#define LCD_TOGGLE  0x02

#define HEL_OK      0x00U
#define HEL_ERROR   0x01U

/**
 * @brief LCD handler structure.
 * 
 * Defines a structure to handle LCD properties and configurations like SPI interface, 
 * pin assignments, and GPIO ports.
 */
typedef struct
{
    SPI_HandleTypeDef *SpiHandler;  /**< SPI handler for communication */
    GPIO_TypeDef *RstPort, *RsPort, *CsPort, *BklPort;  /**< GPIO ports for Reset, RS, CS, and Backlight */
    uint32_t RstPin, RsPin, CsPin, BklPin;  /**< GPIO pins for Reset, RS, CS, and Backlight */
} LCD_HandleTypeDef;

/**
 * @brief Initialize the LCD display.
 * @param hlcd: Pointer to the LCD handle structure.
 * @retval Status: HEL_OK if initialization succeeds, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Init(LCD_HandleTypeDef *hlcd);

/**
 * @brief Initialize the LCD display GPIO/SPI as needed.
 * @param hlcd: Pointer to the LCD handle structure.
 */
void HEL_LCD_MspInit(LCD_HandleTypeDef *hlcd);

/**
 * @brief Send a command to the LCD display.
 * @param hlcd: Pointer to the LCD handle structure.
 * @param cmd: The command to send.
 * @retval Status: HEL_OK if the command is sent successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Command(LCD_HandleTypeDef *hlcd, uint8_t cmd);

/**
 * @brief Send data to the LCD display.
 * @param hlcd: Pointer to the LCD handle structure.
 * @param data: The data to send.
 * @retval Status: HEL_OK if data is sent successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Data(LCD_HandleTypeDef *hlcd, uint8_t data);

/**
 * @brief Display a string on the LCD.
 * @param hlcd: Pointer to the LCD handle structure.
 * @param str: Pointer to the string to display.
 * @retval Status: HEL_OK if the string is displayed successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_String(LCD_HandleTypeDef *hlcd, char *str);

/**
 * @brief Set the cursor position on the LCD.
 * @param hlcd: Pointer to the LCD handle structure.
 * @param row: Row number (0 or 1).
 * @param col: Column number (0 to 15).
 * @retval Status: HEL_OK if the cursor position is set successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_SetCursor(LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col);

/**
 * @brief Control the LCD backlight.
 * @param hlcd: Pointer to the LCD handle structure.
 * @param state: LCD_ON to turn on backlight, LCD_OFF to turn it off, LCD_TOGGLE to toggle it.
 */
void HEL_LCD_Backlight(LCD_HandleTypeDef *hlcd, uint8_t state);

/**
 * @brief Adjust the LCD contrast.
 * @param hlcd: Pointer to the LCD handle structure.
 * @param contrast: The contrast value (0 to 15).
 * @retval Status: HEL_OK if the contrast is adjusted successfully, HEL_ERROR otherwise.
 */
uint8_t HEL_LCD_Contrast(LCD_HandleTypeDef *hlcd, uint8_t contrast);

#endif // __HEL_LCD_H__
