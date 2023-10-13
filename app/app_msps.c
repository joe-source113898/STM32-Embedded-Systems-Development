/**
 * @file auxiliary_initializations.c
 * @brief Functions for auxiliary initializations of the library
 */

#include "app_bsp.h"
#include "hel_lcd.h"

/**
 * @brief HAL MspInit function override
 */
void HAL_MspInit(void)
{
    /* TODO: Implement your MspInit configuration here */
}

/**
 * @brief HAL_FDCAN MspInit function override
 * @param hfdcan FDCAN handle
 */
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan)
{
    GPIO_InitTypeDef GpioCanStruct;

    /* Enable clocks for GPIO and CAN peripherals */
    __HAL_RCC_FDCAN_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    /* Configure Pin 0 (Rx) and Pin 1 (Tx) in alternate mode for FDCAN1 */
    GpioCanStruct.Mode = GPIO_MODE_AF_PP;
    GpioCanStruct.Alternate = GPIO_AF3_FDCAN1;
    GpioCanStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
    GpioCanStruct.Pull = GPIO_NOPULL;
    GpioCanStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GpioCanStruct);

    /* Configure Pin 14 (Rx) and Pin 15 (Tx) in alternate mode for FDCAN2 */
    GpioCanStruct.Mode = GPIO_MODE_AF_PP;
    GpioCanStruct.Alternate = GPIO_AF3_FDCAN2;
    GpioCanStruct.Pin = GPIO_PIN_14 | GPIO_PIN_15;
    GpioCanStruct.Pull = GPIO_NOPULL;
    GpioCanStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOD, &GpioCanStruct);


    /* Enable vector interrupt to handle CAN IRQs */
    HAL_NVIC_SetPriority(TIM16_FDCAN_IT0_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM16_FDCAN_IT0_IRQn);
}

/**
 * @brief HAL_RTC MspInit function override
 * @param hrtc RTC handle
 */
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    __HAL_RCC_SYSCFG_CLK_ENABLE();
    __HAL_RCC_PWR_CLK_ENABLE();

    /* Enable backup domain */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

    /* Reset previous RTC source clock */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_NONE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Configure LSE/LSI as RTC clock source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_OFF;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Set LSE as source clock */
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_RTC_ENABLE();
    __HAL_RCC_RTCAPB_CLK_ENABLE();
}

/**
 * @brief Initializes the SPI1 peripheral and its associated GPIO pins.
 * @param hspi Pointer to the SPI handle structure.
 */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0}; /* Declare and initialize a GPIO configuration structure */
    
    /* Detailed GPIO pin mapping for SPI1: 
        PC5 ------> SPI1_SCK (Serial Clock)
        PC6 ------> SPI1_MISO (Master In, Slave Out)
        PC7 ------> SPI1_MOSI (Master Out, Slave In)
    */

    __HAL_RCC_SPI1_CLK_ENABLE(); /* Enable the clock for the SPI1 peripheral */
    __HAL_RCC_GPIOD_CLK_ENABLE(); /* Enable the clock for the GPIOC port */

    /* Configure the properties for the GPIO pins we are setting up:
        - Pin: Specifies which pins will be configured (in this case PC5, PC6, PC7)
        - Mode: Set the mode to Alternate Function Push-Pull, needed for SPI functionality
        - Pull: Specifies that there are no pull-up or pull-down resistors on the pin
        - Speed: Set the GPIO speed to high. This affects the slew rate of the pin
        - Alternate: Specifies the alternate function for the pin. GPIO_AF5_SPI1 connects the pin to SPI1 functionality 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_5 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;

    /* Use the configuration structure to initialize and configure the pins on GPIOC for SPI1 functionality */
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/**
 * @brief Initializes the peripherals related to the LCD.
 * 
 * @param hlcd Pointer to the LCD handler structure.
 */
void HEL_LCD_MspInit(LCD_HandleTypeDef *hlcd) 
{
    GPIO_InitTypeDef GPIO_LCD_InitStruct = {0};

    /* Enable the GPIO clock for the pins */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /* Initialize the pints of the LCD */
    GPIO_LCD_InitStruct.Pin = hlcd->CsPin | hlcd->RsPin | hlcd->RstPin;
    GPIO_LCD_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_LCD_InitStruct.Pull = GPIO_NOPULL;
    GPIO_LCD_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(hlcd->CsPort, &GPIO_LCD_InitStruct);

    GPIO_LCD_InitStruct.Pin = hlcd->BklPin;
    GPIO_LCD_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_LCD_InitStruct.Pull = GPIO_NOPULL;
    GPIO_LCD_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(hlcd->BklPort, &GPIO_LCD_InitStruct);
    HAL_GPIO_WritePin(hlcd->BklPort, hlcd->BklPin, GPIO_PIN_SET);    
}
