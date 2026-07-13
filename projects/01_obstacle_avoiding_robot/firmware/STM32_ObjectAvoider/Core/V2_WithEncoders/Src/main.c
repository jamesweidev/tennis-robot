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

char msg[128] = "HELLO MATE";

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	TIM2_PWM_Init();
	TIM4_IC_init();
	TIM3_Micros_Init();
	Ultrasonic_GPIO_Init();

	UART3_Init();

	Send_Msg("\r\n\n\n\n\n\n\n\n\n\n");
	while (1)
	{
		uint32_t samples[5] = {0};
		for (uint8_t count = 0; count < 5; count++)
		{
			uint32_t raw_distance = get_distance();

			uint8_t i = 0;
			while (raw_distance < samples[i]) i++;

			uint32_t prev_value = raw_distance;
			for (uint8_t j = i; j < 5; j++)
			{
				uint32_t temp = samples[j];
				samples[j] = prev_value;
				prev_value = temp;
			}
			HAL_Delay(20);
		}

		uint32_t distance = samples[2];

		sprintf(msg, "RPM: %.2f FTarget: %ld CTarget: %ld i_value: %.2f DISTANCE: %lu\r\n", 
			right_encoder.current_rpm,
			right_encoder.final_target_rpm,
			right_encoder.target_rpm,
			right_encoder.pid.i_value,
			distance
		);
		Send_Msg(msg);

		HAL_Delay(200);

		if (distance >= 500 || distance == 0)
		{
			// Sensor times out and returns 0 if It's too far from the closest object
			// So robot should go forward when distance == 0
			Drive_Motor(300, ACTION_FORWARD);
		} else if (distance >= 150)
		{
			Drive_Motor(300, ACTION_RIGHT);
		} else 
		{
			Drive_Motor(300, ACTION_BACKWARD);
			HAL_Delay(100);
		}

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
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
	while (1)
	{
	}
}