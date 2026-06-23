/*
 * 05uart_transmit.c
 *
 *  Created on: May 23, 2026
 *      Author: james
 */


#include "stm32f446xx.h"
#include <string.h>


// 115200 BAUDRATE, 1 stop bit, 8 bit frame, no parity
USART_Handle_t uart1_handle;
char* data = "what the helly in the sauce gangy\n";

void gpio_inits(void)
{
	GPIO_Config_t config;
	GPIO_Handle_t pHandle;

	pHandle.pGPIOx = GPIOA;

	GPIO_ClockControl(pHandle.pGPIOx, 1);

	config.GPIO_PinAltFuncMode = 7;
	config.GPIO_PinMode = 2;
	config.GPIO_PinOPType = 0;
	config.GPIO_PinPuPdControl = 0;
	config.GPIO_PinSpeed = 2;

	config.GPIO_PinNumber = 9;
	pHandle.GPIOConfig = config;
	GPIO_Init(&pHandle);

	config.GPIO_PinNumber = 10;
	pHandle.GPIOConfig = config;
	GPIO_Init(&pHandle);


	// Button
	config.GPIO_PinMode = 0;
	config.GPIO_PinNumber = 7;
	config.GPIO_PinPuPdControl = 2;
	GPIO_Init(&pHandle);
}

void uart_inits()
{
	USART_Config_t config;

	config.baud = 4;
	config.word_len = 0;
	config.parity = 0;
	config.over8 = 0;
	config.stop_bits = 0;
	config.flow_ctrl = 0;
	config.mode = 0;

	uart1_handle.config = config;
	uart1_handle.pUSARTx = USART1;

	usart_init(&uart1_handle);
}

int main(void)
{

	gpio_inits();

	uart_inits();

	systick_enable(16000000);


	while (1)
	{
		while (! GPIO_ReadFromInputPin(GPIOA, 7));

		delay(100);
		usart_send_data(&uart1_handle, (uint8_t *) data, strlen(data));

		while (GPIO_ReadFromInputPin(GPIOA, 7));
	}


	return 0;
}
















