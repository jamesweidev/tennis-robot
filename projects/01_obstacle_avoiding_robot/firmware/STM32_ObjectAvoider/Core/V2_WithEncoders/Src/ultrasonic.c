#include "main.h"


TIM_HandleTypeDef htim3 = {0};

void Ultrasonic_GPIO_Init(void)
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpio_init = {0};

	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;
	
	// Configuring Trig
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pin = US_TRIG_PIN;
	HAL_GPIO_Init(US_GPIO_PORT, &gpio_init);

	// Configuring Echo
	gpio_init.Mode = GPIO_MODE_INPUT;
	gpio_init.Pin = US_ECHO_PIN;
	HAL_GPIO_Init(US_GPIO_PORT, &gpio_init);
}

uint32_t get_distance(void)
{
	// Sends initial pulse
	HAL_GPIO_WritePin(US_GPIO_PORT, US_TRIG_PIN, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(US_GPIO_PORT, US_TRIG_PIN, GPIO_PIN_RESET);


	uint32_t timeout_start = micros();
	// Wait until pulse returns, then start counting. if timedout, return 0
	while (HAL_GPIO_ReadPin(US_GPIO_PORT, US_ECHO_PIN) != GPIO_PIN_SET)
	{
		if (micros() - timeout_start >= 38000)
		{
			return 0;
		}
	}

	uint32_t pulse_start = micros();
	// Wait until pulse ends, return the pulse length
	while (HAL_GPIO_ReadPin(US_GPIO_PORT, US_ECHO_PIN) == GPIO_PIN_SET)
	{
		if (micros() - pulse_start >= 38000)
		{
			return 0;
		}
	}

	uint32_t duration_us = micros() - pulse_start;

	// get the distance via time elapsed * speed of sound
	uint32_t distance = duration_us * 343 / 2000; // 0.343 mm/us


	return distance;
}

void TIM3_Micros_Init(void)
{
	htim3.Instance = TIM3;
	htim3.Init.Period = 0xFFFFFFFF;
	htim3.Init.Prescaler = 50 - 1; // 1 MHz clock, 1 µs per tick

	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_Base_Start(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
}

uint32_t micros(void)
{
	return __HAL_TIM_GET_COUNTER(&htim3);
}

void delay_us(uint32_t us)
{
	uint32_t start_micros = micros();
	while (micros() - start_micros < us);
}