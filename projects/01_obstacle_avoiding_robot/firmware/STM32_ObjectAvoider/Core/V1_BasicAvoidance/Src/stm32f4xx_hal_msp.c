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
        gpio_init.Pin = PWM_RIGHT_PIN;
        HAL_GPIO_Init(PWM_RIGHT_GPIO_PORT, &gpio_init);

        // Left PWM Channel Pin
        gpio_init.Pin = PWM_LEFT_PIN;
        HAL_GPIO_Init(PWM_LEFT_GPIO_PORT, &gpio_init);

        // Configure motor driver direction pins
        // IN1
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
        gpio_init.Pull = GPIO_NOPULL;
        gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init.Pin = IN1_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
        // IN2
        gpio_init.Pin = IN2_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
        // IN3
        gpio_init.Pin = IN3_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
        // IN4
        gpio_init.Pin = IN4_PIN;
        HAL_GPIO_Init(MOTOR_DRIVER_IN_PORT, &gpio_init);
    }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    __HAL_RCC_TIM3_CLK_ENABLE();
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    // Enable USART1 and gpioa clock
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // Enable TX and RX GPIOs and set the appropriate alternate function
    GPIO_InitTypeDef gpio_config = {0};
    gpio_config.Alternate = GPIO_AF7_USART1;
    gpio_config.Mode = GPIO_MODE_AF_PP;
    gpio_config.Pull = GPIO_NOPULL;
    gpio_config.Speed = GPIO_SPEED_FREQ_HIGH;

    gpio_config.Pin = USART1_TX_PIN;
    HAL_GPIO_Init(USART1_TX_PORT, &gpio_config);

    // Enable UART interrupts
    HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}
