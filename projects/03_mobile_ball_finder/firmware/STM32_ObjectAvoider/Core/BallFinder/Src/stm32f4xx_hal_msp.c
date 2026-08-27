#include "main.h"
#include "stm32_hal_legacy.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_cortex.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_gpio_ex.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_rcc_ex.h"

void HAL_MspInit(void)
{
    // 1. set up priority grouping
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    // 2. Enable required system exceptions
    SCB->SHCSR |= 0x7 << 16; // enable usage fault, memory fault, and bus fault system exceptions
    // 3. configure the prioirty for the system exceptions

}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_TIM2_CLK_ENABLE();

        GPIO_InitTypeDef gpio_init = {0};

        gpio_init.Alternate = GPIO_AF1_TIM2;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_NOPULL;
        gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

        // Right motor control pins
        gpio_init.Pin = AIN1_PIN;
        HAL_GPIO_Init(RIGHT_MOTOR_DRIVER_PORT, &gpio_init);
        gpio_init.Pin = AIN2_PIN;
        HAL_GPIO_Init(RIGHT_MOTOR_DRIVER_PORT, &gpio_init);

        // Left motor control pins
        gpio_init.Pin = BIN1_PIN;
        HAL_GPIO_Init(LEFT_MOTOR_DRIVER_PORT, &gpio_init);
        gpio_init.Pin = BIN2_PIN;
        HAL_GPIO_Init(LEFT_MOTOR_DRIVER_PORT, &gpio_init);
    }
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4)
    {
        // Right encoder pins
        __HAL_RCC_TIM4_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitTypeDef gpio_init = {0};

        gpio_init.Alternate = GPIO_AF2_TIM4;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_NOPULL;

        // RIGHT ENA
        gpio_init.Pin = RIGHT_ENCA_PIN;
        HAL_GPIO_Init(RIGHT_ENCA_PORT, &gpio_init);

        // RIGHT ENB
        gpio_init.Pin = RIGHT_ENCB_PIN;
        HAL_GPIO_Init(RIGHT_ENCB_PORT, &gpio_init);

    } else if (htim->Instance == TIM3)
    {
        // Left encoder pins
        __HAL_RCC_TIM3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitTypeDef gpio_init = {0};

        gpio_init.Alternate = GPIO_AF2_TIM3;
        gpio_init.Mode = GPIO_MODE_AF_PP;
        gpio_init.Pull = GPIO_NOPULL;

        // ENA
        gpio_init.Pin = LEFT_ENCA_PIN;
        HAL_GPIO_Init(LEFT_ENCA_PORT, &gpio_init);

        // ENB
        gpio_init.Pin = LEFT_ENCB_PIN;
        HAL_GPIO_Init(LEFT_ENCB_PORT, &gpio_init);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        __HAL_RCC_TIM6_CLK_ENABLE();
    } else if (htim->Instance == TIM7)
    {
        __HAL_RCC_TIM7_CLK_ENABLE();

        HAL_NVIC_SetPriority(TIM7_IRQn, 15, 0);
        HAL_NVIC_EnableIRQ(TIM7_IRQn);
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        // Debug UART msp init

        // Enable USART2 and GPIOB clock
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
    
        // Enable TX and RX GPIOs and set the appropriate alternate function
        GPIO_InitTypeDef gpio_config = {0};
        gpio_config.Alternate = GPIO_AF7_USART2;
        gpio_config.Mode = GPIO_MODE_AF_PP;
        gpio_config.Pull = GPIO_NOPULL;
        gpio_config.Speed = GPIO_SPEED_FREQ_MEDIUM;
    
        gpio_config.Pin = USART2_TX_PIN;
        HAL_GPIO_Init(USART2_TX_PORT, &gpio_config);
    
        // Enable UART interrupts
        HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    
        // Debugging Onboard LED
        gpio_config.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_config.Pin = GPIO_PIN_5;
        HAL_GPIO_Init(GPIOA, &gpio_config);
    } else if (huart->Instance == USART6)
    {
        // Command receive uart msp init

        __HAL_RCC_USART6_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();

        GPIO_InitTypeDef gpio_config = {0};
        gpio_config.Alternate = GPIO_AF8_USART6;
        gpio_config.Mode = GPIO_MODE_AF_PP;
        gpio_config.Pull = GPIO_NOPULL;
        gpio_config.Speed = GPIO_SPEED_FREQ_MEDIUM;

        gpio_config.Pin = UART6_TX_PIN;
        HAL_GPIO_Init(UART6_GPIO_PORT, &gpio_config);
        gpio_config.Pin = UART6_RX_PIN;
        HAL_GPIO_Init(UART6_GPIO_PORT, &gpio_config);

        // Enable UART interrupts
        HAL_NVIC_SetPriority(USART6_IRQn, 14, 0);
        HAL_NVIC_EnableIRQ(USART6_IRQn);
    }
}
