#include "main.h"
#include "stm32f4xx_hal_i2c.h"
#include "stm32f4xx_hal_tim.h"



void SystemClock_Config(void);
void Error_Handler(void);

// debug
void GPIO_init(void);
void UART1_init(void);
UART_HandleTypeDef huart1 = {0};

// IMU
void IMU_I2C_Init(void);

char msg[128];

int main(void)
{
	HAL_Init();
	SystemClock_Config(); // 50MHz Sysclk, 50MHz AHB, 25MHz APB1, 50MHz APB2

	// Debugging
	GPIO_init();
	UART1_init();

	while (1)
	{
		sprintf(msg, "RPM diff: %.1f Offset: %ld Seconds: %ld\r\n\n", (target_rpm - current_rpm), offset, seconds);
		HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), 100);

		HAL_Delay(1000);
		seconds++;
	}

	return 0;
}

void IMU_I2C_Init(void)
{
	I2C_InitTypeDef i2c_init;

	i2c_init.AddressingMode
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


// Debuging

void Error_Handler(void)
{
	while (1);
}

void GPIO_init(void)
{
	// Initialize Onboard User LED for debugging
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpio_init = {0};

	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pin = USR_LED_PIN;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(USR_LED_PORT, &gpio_init);
}

void UART1_init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.Mode = UART_MODE_TX;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;

	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}