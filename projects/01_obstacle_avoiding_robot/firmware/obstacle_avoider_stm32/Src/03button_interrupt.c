/*
 * 03button_interrupt.c
 *
 *  Created on: May 13, 2026
 *      Author: james
 */

#include "stm32f446xx_gpio_driver.h"

int main(void)
{
	GPIO_Handle_t GPIO_led, GPIO_button;
	GPIO_Config_t led_config, button_config;

	GPIO_led.pGPIOx = GPIOA;
	GPIO_button.pGPIOx = GPIOB;

	led_config.GPIO_PinMode = GPIO_PIN_MODE_OUT;
	led_config.GPIO_PinNumber = 5;
	led_config.GPIO_PinOPType = GPIO_OT_PP;
	led_config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	led_config.GPIO_PinSpeed = GPIO_OSPEED_LOW;
	led_config.GPIO_PinAltFuncMode = 0;

	button_config.GPIO_PinMode = GPIO_PIN_MODE_IT_RT;
	button_config.GPIO_PinNumber = 2;
	led_config.GPIO_PinOPType = GPIO_OT_PP;
	button_config.GPIO_PinPuPdControl = GPIO_PUPD_PD;
	button_config.GPIO_PinSpeed = GPIO_OSPEED_FAST;
	button_config.GPIO_PinAltFuncMode = 0;

	GPIO_led.GPIOConfig = led_config;
	GPIO_button.GPIOConfig = button_config;

	GPIO_ClockControl(GPIOA, ENABLE);
	GPIO_ClockControl(GPIOB, ENABLE);

	GPIO_Init(&GPIO_led);
	GPIO_Init(&GPIO_button);


	GPIO_IRQConfig(IRQ_NO_EXTI2, ENABLE);

	while (1);

	return 0;
}





void EXTI2_IRQHandler(void)
{
	GPIO_IRQHandling(2);

	GPIO_TogglePin(GPIOA, 5);
}














