#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"


extern Encoder right_encoder;
extern Encoder left_encoder;

extern UART_HandleTypeDef huart3;

char msg[64];

int main(void)
{
	HAL_Init();
	SystemClock_Config();

	// TIM3_Micros_Init();
	TIM2_PWM_Init();
	TIM4_IC_init();
	// Ultrasonic_GPIO_Init();

	UART3_Init();

	while (1)
	{
		// uint32_t distance = get_distance();

		// if (distance == 0)
		// {
		// 	for (uint8_t num_zeros = 0; num_zeros < 5; num_zeros++)
		// 	{
		// 		// Sensor randomly returns 0
		// 		// If that happens just get the distance a couple more times
		// 		distance = get_distance();
		// 	}
		// }

		Perform_Action(75, ACTION_FORWARD);


		sprintf(msg, "Left RPM: %.2f Right RPM: %.2f\r\n", left_encoder.current_rpm, right_encoder.current_rpm);
		if (HAL_UART_Transmit(&huart3, (uint8_t*) msg, strlen(msg), 0xFFFF) != HAL_OK)
		{
			Error_Handler();
		}

		HAL_Delay(1000);
		// if (distance >= 500 || distance == 0)
		// {
		// 	// Sensor times out and returns 0 if It's too far from the closest object
		// 	// So robot should go forward when distance == 0
		// 	Perform_Action(65, ACTION_FORWARD);
		// } else if (distance >= 150)
		// {
		// 	Perform_Action(65, ACTION_RIGHT);
		// } else 
		// {
		// 	Perform_Action(75, ACTION_BACKWARD);
		// 	HAL_Delay(100);
		// }

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
	while (1);
}