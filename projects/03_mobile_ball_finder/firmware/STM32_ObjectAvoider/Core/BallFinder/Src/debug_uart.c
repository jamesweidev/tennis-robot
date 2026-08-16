#include "main.h"
#include "stm32f4xx_hal_def.h"

UART_HandleTypeDef huart2 = {0};

void Debug_UART2_Init(void)
{
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.Mode = UART_MODE_TX;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.StopBits = UART_STOPBITS_1;

	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
}

int _write(int file, char* data, int len)
{
	if (HAL_UART_Transmit(&huart2, (uint8_t*) data, (uint16_t) len, HAL_MAX_DELAY) != HAL_OK)
	{
		return -1;
	} 

	return len;
}