#include "main.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_uart.h"
#include <stdio.h>


extern Encoder right_encoder;
extern Encoder left_encoder;

extern UART_HandleTypeDef huart3;

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	TIM3_Micros_Init();
	Ultrasonic_GPIO_Init();
	TIM2_PWM_Init();
	TIM4_IC_init();

	Debug_UART3_Init();

	while (1)
	{
		Update_State();
	}
	return 0;
}



void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc_init = {0};
    RCC_PLLInitTypeDef pll_init = {0};

    osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_init.HSEState = RCC_HSE_BYPASS;

    pll_init.PLLState = RCC_PLL_ON;
    pll_init.PLLSource = RCC_PLLSOURCE_HSE;
    pll_init.PLLM = 8;
    pll_init.PLLN = 100;
    pll_init.PLLP = 2;
    pll_init.PLLR = 2;
    pll_init.PLLQ = 2;

    osc_init.PLL = pll_init;

    if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitTypeDef clk_init = {0};
    clk_init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_SYSCLK;
    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk_init.APB1CLKDivider = RCC_HCLK_DIV1;
    clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
}

void Error_Handler(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	while (1)
	{
	}
}