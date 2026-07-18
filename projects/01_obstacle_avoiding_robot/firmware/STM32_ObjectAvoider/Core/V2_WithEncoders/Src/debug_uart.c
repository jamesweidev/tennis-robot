#include "main.h"
#include "stm32f4xx_hal_def.h"

UART_HandleTypeDef huart3 = {0};

void UART3_Init(void)
{
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.Mode = UART_MODE_TX;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.StopBits = UART_STOPBITS_1;

	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
}

int _write(int file, char* data, int len)
{
	if (HAL_UART_Transmit(&huart3, (uint8_t*) data, (uint16_t) len, HAL_MAX_DELAY) != HAL_OK)
	{
		return -1;
	} 

	return len;
}