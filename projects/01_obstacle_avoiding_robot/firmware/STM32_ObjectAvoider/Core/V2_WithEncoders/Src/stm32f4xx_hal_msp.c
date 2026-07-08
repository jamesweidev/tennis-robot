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
    // 3. configure the prioirty fo rthe system exceptions

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

        // Right PWM Channel Pin
        gpio_init.Pin = PWMA_PIN;
        HAL_GPIO_Init(PWMA_PORT, &gpio_init);

        // Left PWM Channel Pin
        gpio_init.Pin = PWMB_PIN;
        HAL_GPIO_Init(PWMB_PORT, &gpio_init);

        // Configure motor driver direction pins
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init.Pull = GPIO_NOPULL;
        gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
        // AIN1
        gpio_init.Pin = AIN1_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
        // IN2
        gpio_init.Pin = AIN2_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
        // IN3
        gpio_init.Pin = BIN1_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
        // IN4
        gpio_init.Pin = BIN2_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
    }
}

void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    // Initialize encoder pins
    __HAL_RCC_TIM2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init;

    gpio_init.Alternate = GPIO_AF1_TIM2;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;

    // RIGHT ENA
    gpio_init.Pin = RIGHT_ENCA_PIN;
    HAL_GPIO_Init(RIGHT_ENCA_PORT, &gpio_init);

    // RIGHT ENB (not used for interrupt)
    gpio_init.Pin = RIGHT_ENCB_PIN;
    gpio_init.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(RIGHT_ENCA_PORT, &gpio_init);

    // LEFT ENA
    gpio_init.Pin = LEFT_ENCA_PIN;
    HAL_GPIO_Init(LEFT_ENCA_PORT, &gpio_init);

    // LEFT ENB (not used for interrupt)
    gpio_init.Pin = LEFT_ENCB_PIN;
    gpio_init.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(LEFT_ENCA_PORT, &gpio_init);

    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // Enable USART3 and GPIOB clock
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Enable TX and RX GPIOs and set the appropriate alternate function
    GPIO_InitTypeDef gpio_config = {0};
    gpio_config.Alternate = GPIO_AF7_USART3;
    gpio_config.Mode = GPIO_MODE_AF_PP;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_MEDIUM;

    gpio_config.Pin = USART3_TX_PIN;
    HAL_GPIO_Init(USART3_TX_PORT, &gpio_config);

    // Enable UART interrupts
    HAL_NVIC_SetPriority(USART3_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
}
