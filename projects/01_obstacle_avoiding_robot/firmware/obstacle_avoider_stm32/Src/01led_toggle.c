/*
 * 01led_toggle.c
 *
 *  Created on: May 11, 2026
 *      Author: james
 */

#include "stm32f446xx_gpio_driver.h"
#include "stm32f446xx_systick_driver.h"


int main(void)
{

	GPIO_Handle_t GPIO_led;
	GPIO_Config_t led_config;

	led_config.GPIO_PinSpeed = 0;
	led_config.GPIO_PinNumber = 5;
	led_config.GPIO_PinMode = 1;
	led_config.GPIO_PinOPType = 0;
	led_config.GPIO_PinPuPdControl = 0;
	led_config.GPIO_PinAltFuncMode = 0;

	GPIO_led.pGPIOx = GPIOA;
	GPIO_led.GPIOConfig = led_config;

	GPIO_ClockControl(GPIO_led.pGPIOx, true);
	GPIO_Init(&GPIO_led);

	systick_enable(16000000);
	while (1)
	{
		GPIO_WriteToOutputPin(GPIO_led.pGPIOx, 5, 1);
		delay(1000);
		GPIO_WriteToOutputPin(GPIO_led.pGPIOx, 5, 0);
		delay(1000);
	}

	return 0;
}
