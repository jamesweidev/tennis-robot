


#include "stm32f446xx.h"



void pwm_init(void)
{
	// Enable TIM2 clock
	TIM2_CLK_EN();
	// Configure PSC, ARR, disable counter, set CNT to 0,
	TIM2->CR1 &= ~(1);

	TIM2->ARR = 999;
	TIM2->PSC = 15;
	TIM2->CNT = 0;

	uint32_t tempreg = 0;
	tempreg |= (6 << 0x4);
	TIM2->CCMR1 = tempreg;

	TIM2->CCR1 = 500; // 50% duty cycle

	TIM2->CCMR1 &= ~(0x3); // Configure Channel 1 as output

	TIM2->CR1 |= 1;

	TIM2->CCER |= 1;
}

void pwm_gpio_init(void)
{
	GPIO_Handle_t handle;
	GPIO_Config_t config;

	handle.pGPIOx = GPIOA;

	config.GPIO_PinAltFuncMode = 1;
	config.GPIO_PinMode = GPIO_PIN_MODE_AF;
	config.GPIO_PinNumber = 0;
	config.GPIO_PinOPType = GPIO_OT_PP;
	config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	config.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	handle.GPIOConfig = config;

	GPIO_Init(&handle);
}


void set_duty_cycle(uint32_t duty)
{
	TIM2->CCR1 = duty;
}


int main(void)
{
	pwm_gpio_init();
	pwm_init();
	systick_enable(16000000);

	uint32_t current_duty = 1000;

	while (1)
	{
		delay(1000);
		set_duty_cycle(0);
		delay(1000);
		set_duty_cycle(current_duty);
	}

	return 0;
}





