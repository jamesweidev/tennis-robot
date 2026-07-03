#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"
#include <stdio.h>
#include <string.h>


TIM_HandleTypeDef htim2 = {0};
TIM_HandleTypeDef htim3 = {0};
UART_HandleTypeDef huart1 = {0};

void Perform_Action(uint32_t speed, ActionType type);
void motor_direction_config(ActionType type);
void Stop_Robot();
uint32_t get_distance(void);
uint32_t micros(void);
void delay_us(uint32_t micros);

void Ultrasonic_GPIO_Init(void);
void TIM3_Micros_Init(void);
void PWM_Init(void);
void SystemClock_Config(void);

void UART1_Init(void);
void Error_Handler(void);

char msg[64];

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	TIM3_Micros_Init();
	PWM_Init();
	Ultrasonic_GPIO_Init();

	UART1_Init();

	uint32_t zeros = 0;
	while (1)
	{
		uint32_t distance = get_distance();
		if (distance == 0)
		{
			distance = get_distance();

			if (distance == 0) 
			{
				zeros++;
			}
		}

		// sprintf(msg, "distance is: %lu\r\n", distance);
		// if (HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), 0xFFFFFFFF) != HAL_OK)
		// {
		// 	Error_Handler();
		// }

		if ((distance < 150 && distance != 0))
		{
			Perform_Action(67, ACTION_BACKWARD);
		} else if (zeros >= 100)
		{
			Perform_Action(67, ACTION_BACKWARD);
			HAL_Delay(600);
			zeros = 0;
		}else if (distance >= 500 || distance == 0)
		{
			Perform_Action(67, ACTION_FORWARD);
		} else
		{
			Perform_Action(67, ACTION_RIGHT);
		}

	}
	return 0;
}

void Perform_Action(uint32_t speed, ActionType type)
{
	motor_direction_config(type);

	speed = PWM_PERIOD * speed / 100;

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, speed);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, speed);
}

void PWM_Init(void)
{
	TIM_OC_InitTypeDef pwm_init = {0};

	// Period of 20kHz
	htim2.Instance = TIM2;
	htim2.Init.Period = PWM_PERIOD - 1;
	htim2.Init.Prescaler = 1 - 1;

	pwm_init.Pulse = 0;
	pwm_init.OCMode = TIM_OCMODE_PWM1;
	pwm_init.OCPolarity = TIM_OCPOLARITY_HIGH;

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	// Channel 1 (Right PWM) config
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}

	// Channel 2 (Left PWM) config
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2) != HAL_OK)
	{
		Error_Handler();
	}
}

void Stop_Robot(void)
{
	Perform_Action(0, ACTION_STOP);
}

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

void motor_direction_config(ActionType type)
{
	if (type == ACTION_FORWARD)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN2_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN3_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN4_PIN, GPIO_PIN_SET);
	} else if (type == ACTION_BACKWARD)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN2_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN3_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN4_PIN, GPIO_PIN_RESET);
	} else if (type == ACTION_LEFT)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN2_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN3_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN4_PIN, GPIO_PIN_RESET);
	} else if (type == ACTION_RIGHT)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN2_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN3_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN4_PIN, GPIO_PIN_SET);
	} else if (type == ACTION_STOP)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN2_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN3_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, IN4_PIN, GPIO_PIN_RESET);
	}
}

uint32_t get_distance(void)
{
	HAL_GPIO_WritePin(US_GPIO_PORT, US_TRIG_PIN, GPIO_PIN_SET);
	delay_us(10);
	HAL_GPIO_WritePin(US_GPIO_PORT, US_TRIG_PIN, GPIO_PIN_RESET);


	uint32_t timeout_start = micros();
	// Wait until pulse, then start counting. if timedout, return 0
	
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

void UART1_Init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.Mode = UART_MODE_TX;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.StopBits = UART_STOPBITS_1;

	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

void Error_Handler(void)
{
	while (1);
}