#ifndef __APP_SPI_LCD_H__
#define __APP_SPI_LCD_H__

/**
 * @file    app_spi_lcd.h
 * @brief   Header file of the app for testing an LCD driver.
 */

/* Pins/Ports to control the LCD */
#define LCD_CS_PIN GPIO_PIN_3 /*<< Chip select LCD*/
#define LCD_CS_PORT GPIOD
#define LCD_CLK_PIN GPIO_PIN_8 /*<< Serial clock*/
#define LCD_CLK_PORT GPIOD
#define LCD_RS_PIN GPIO_PIN_4 /*<< Data/Command */
#define LCD_RS_PORT GPIOD
#define LCD_RST_PIN GPIO_PIN_2 /*<< Reset */
#define LCD_RST_PORT GPIOD
#define LCD_BL_PIN GPIO_PIN_4 /*<< Backlight */
#define LCD_BL_PORT GPIOB

/* Pins/Ports to handle the SPI communication */
#define SPI_SI_PIN GPIO_PIN_6 /*<< MISO*/
#define SPI_SI_PORT GPIOD
#define SPI_SO_PIN GPIO_PIN_5 /*<< MOSI*/
#define SPI_SO_PORT GPIOD
#define SPI_CS_SPI_PIN GPIO_PIN_9 /*<< Chip select */
#define SPI_CS_SPI_PORT GPIOD

/**
 * @brief Initialize the SPI (Serial Peripheral Interface).
 */
void Init_SPI(void);

#endif // __APP_SPI_LCD_H__
