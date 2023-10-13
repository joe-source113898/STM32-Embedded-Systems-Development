/**
 * @file    app_spi_lcd.c
 * @brief   App for testing an LCD driver.
 */

#include "app_bsp.h"
#include "hel_lcd.h"
#include "app_spi_lcd.h"

extern SPI_HandleTypeDef SpiHandle;

/**
 * @brief Initialize the SPI (Serial Peripheral Interface).
 */
void Init_SPI(void)
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
}
