/*
 * 02external_button.c
 *
 *  Created on: May 12, 2026
 *      Author: james
 */


#include "stm32f446xx_gpio_driver.h"


void delay()
{
	for (volatile uint32_t i = 0; i < 5000000; i++);
}

int main(void)
{
	GPIO_Handle_t GPIO_led, GPIO_button;
	GPIO_Config_t GPIO_led_config, GPIO_button_config;

	GPIO_led.pGPIOx = GPIOA;
	GPIO_button.pGPIOx = GPIOA;

	GPIO_led_config.GPIO_PinAltFuncMode = 0;
	GPIO_led_config.GPIO_PinMode = 1;
	GPIO_led_config.GPIO_PinNumber = 5;
	GPIO_led_config.GPIO_PinOPType = 0;
	GPIO_led_config.GPIO_PinPuPdControl = 0;
	GPIO_led_config.GPIO_PinSpeed = 0;

	GPIO_button_config.GPIO_PinAltFuncMode = 0;
	GPIO_button_config.GPIO_PinMode = 0;
	GPIO_button_config.GPIO_PinNumber = 12;
	GPIO_button_config.GPIO_PinOPType = 0;
	GPIO_button_config.GPIO_PinPuPdControl = 2;
	GPIO_button_config.GPIO_PinSpeed = 0;


	GPIO_led.GPIOConfig = GPIO_led_config;
	GPIO_button.GPIOConfig = GPIO_button_config;

	GPIO_ClockControl(GPIO_led.pGPIOx, 1);

	GPIO_Init(&GPIO_led);
	GPIO_Init(&GPIO_button);


	while (1)
	{
		uint8_t pin_value = GPIO_ReadFromInputPin(
				GPIO_button.pGPIOx,
				GPIO_button.GPIOConfig.GPIO_PinNumber
		);

		if (pin_value == 1)
		{
			GPIO_TogglePin(GPIO_led.pGPIOx, GPIO_led.GPIOConfig.GPIO_PinNumber);
			delay();

			pin_value = GPIO_ReadFromInputPin(
					GPIO_button.pGPIOx,
					GPIO_button.GPIOConfig.GPIO_PinNumber
			);

			while (pin_value);
		}
	}


	return 0;
}











