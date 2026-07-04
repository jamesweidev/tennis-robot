#include "main.h"

void HAL_MspInit(void)
{
    // 1. set up priority grouping
    HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
    // 2. Enable required system exceptions
    SCB->SHCSR |= 0x7 << 16; // enable usage fault, memory fault, and bus fault system exceptions
    // 3. enable floating point
    SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2)); /* set CP10 and CP11 Full Access */

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

    // Encoder A
    gpio_init.Pin = ENCA_PIN;
    HAL_GPIO_Init(ENCA_PORT, &gpio_init);

    // Encoder B (Not yet used for interrupt)
    gpio_init.Pin = ENCB_PIN;
    gpio_init.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(ENCA_PORT, &gpio_init);

    HAL_NVIC_SetPriority(TIM2_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    // Initialize PWM output pin
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init = {0};

    gpio_init.Alternate = GPIO_AF2_TIM3;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pin = GPIO_PIN_6;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;

    HAL_GPIO_Init(GPIOA, &gpio_init);

    // Initialize motor driver direction pins
    // IN1 - PA8
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pin = IN1_PIN;
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(IN1_PORT, &gpio_init);

    // IN2 - PA10
    gpio_init.Pin = IN2_PIN;
    HAL_GPIO_Init(IN2_PORT, &gpio_init);
}

// Debugging
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // Initialize the Tx Pin to PA9
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef gpio_init = {0};

    gpio_init.Alternate = GPIO_AF7_USART1;
    gpio_init.Mode = GPIO_MODE_AF_PP;
    gpio_init.Pin = DEBUG_UART_TX_PIN;
    gpio_init.Pull = GPIO_NOPULL;
    gpio_init.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(GPIOA, &gpio_init);
}