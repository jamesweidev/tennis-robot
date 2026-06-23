#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
#include "lcd.h"


GPIO_Handle_t gpio_handle;


void gpio_inits(void)
{
	GPIO_Config_t config;
	gpio_handle.pGPIOx = GPIOA;

	GPIO_ClockControl(gpio_handle.pGPIOx, 1);

	config.GPIO_PinAltFuncMode = 0;
	config.GPIO_PinOPType = GPIO_OT_PP;
	config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	config.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Configuring Trig
	config.GPIO_PinMode = GPIO_PIN_MODE_OUT;
	config.GPIO_PinNumber = 8;
	gpio_handle.GPIOConfig = config;

	GPIO_Init(&gpio_handle);

	// Configuring Echo
	config.GPIO_PinMode = GPIO_PIN_MODE_IN;
	config.GPIO_PinNumber = 9;
	gpio_handle.GPIOConfig = config;
	GPIO_Init(&gpio_handle);
}

uint32_t get_distance(void)
{
	GPIO_WriteToOutputPin(gpio_handle.pGPIOx, 8, 1);
	delay_us(10);
	GPIO_WriteToOutputPin(gpio_handle.pGPIOx, 8, 0);


	uint32_t timeout_start = micros();
	while (! GPIO_ReadFromInputPin(gpio_handle.pGPIOx, 9))
	{
		if (micros() - timeout_start >= 38000)
		{
			return 0;
		}
	}

	uint32_t pulse_start = micros();

	while (GPIO_ReadFromInputPin(gpio_handle.pGPIOx, 9))
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

void num_to_str(uint32_t num, char* buffer)
{
	if (num == 0)
	{
		*buffer = '0';
		buffer++;
		*buffer = '\0';

		return;
	}

	uint8_t digits = 0;
	uint32_t num_cpy = num;

	while (num_cpy > 0)
	{
		num_cpy /= 10;
		digits++;
	}

	uint8_t divisor = 1;

	for (uint8_t i = 1; i < digits; i++)
	{
		divisor *= 10;
	}

	while (divisor > 0)
	{
		char num_as_char = (num / divisor) % 10 + '0';

		*buffer = num_as_char;
		buffer++;
		divisor /= 10;
	}

	*buffer = '\0';
}

char* distance_str = "Distance: ";
char value[10];

char* tempstr = "Number: ";

int main(void)
{
	systick_enable(16000000);
	timer_init();

	printf_init();

	gpio_inits();

	lcd_init();


	while (1)
	{
		uint32_t distance = get_distance();
		delay(1000);

		num_to_str(distance, value);

		lcd_clear();
		lcd_write((uint8_t*) distance_str, strlen(distance_str));
		lcd_write((uint8_t*) value, strlen(value));
		lcd_write(" mm", 3);
	}

	return 0;
}






