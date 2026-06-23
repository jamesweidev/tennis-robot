#include <stdio.h>
#include <string.h>
#include "stm32f446xx.h"
//#include "lcd.h"

#define US_GPIO_PORT			GPIOA
#define US_TRIG_PIN				8
#define US_ECHO_PIN 			9

#define PWM_RIGHT_GPIO_PORT		GPIOA
#define PWM_RIGHT_PIN			6

#define PWM_LEFT_GPIO_PORT		GPIOB
#define PWM_LEFT_PIN			6

#define MOTOR_DRIVER_IN_PORT	GPIOA

#define IN1_PIN					0
#define IN2_PIN					1
#define IN3_PIN					4
#define IN4_PIN					5

#define LEFT					TIM4
#define RIGHT					TIM3

// Directions
#define FORWARD					0
#define BACKWARD				1

GPIO_Handle_t gpio_handle;


void us_gpio_inits(void)
{
	GPIO_Config_t config;
	gpio_handle.pGPIOx = US_GPIO_PORT;

	GPIO_ClockControl(gpio_handle.pGPIOx, 1);

	config.GPIO_PinAltFuncMode = 0;
	config.GPIO_PinOPType = GPIO_OT_PP;
	config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	config.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	// Configuring Trig
	config.GPIO_PinMode = GPIO_PIN_MODE_OUT;
	config.GPIO_PinNumber = US_TRIG_PIN;
	gpio_handle.GPIOConfig = config;

	GPIO_Init(&gpio_handle);

	// Configuring Echo
	config.GPIO_PinMode = GPIO_PIN_MODE_IN;
	config.GPIO_PinNumber = US_ECHO_PIN;
	gpio_handle.GPIOConfig = config;
	GPIO_Init(&gpio_handle);
}

uint32_t get_distance(void)
{
	GPIO_WriteToOutputPin(gpio_handle.pGPIOx, US_TRIG_PIN, 1);
	delay_us(10);
	GPIO_WriteToOutputPin(gpio_handle.pGPIOx, US_TRIG_PIN, 0);


	uint32_t timeout_start = micros();
	// Wait until pulse, then start counting. if timedout, return 0
	while (! GPIO_ReadFromInputPin(gpio_handle.pGPIOx, US_ECHO_PIN))
	{
		if (micros() - timeout_start >= 38000)
		{
			return 0;
		}
	}

	uint32_t pulse_start = micros();
	// Wait until pulse ends, return the pulse length
	while (GPIO_ReadFromInputPin(gpio_handle.pGPIOx, US_ECHO_PIN))
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

void pwm_init(void)
{
	// RIGHT SIDE
	// Enable TIM3 clock
	TIM3_CLK_EN();
	// Configure PSC, ARR, disable counter, set CNT to 0,
	TIM3->CR1 &= ~(1);

	TIM3->ARR = 999;
	TIM3->PSC = 15;
	TIM3->CNT = 0;

	uint32_t tempreg = 0;
	tempreg |= (6 << 0x4);
	TIM3->CCMR1 = tempreg;

	TIM3->CCR1 = 0; // 0% duty cycle

	TIM3->CCMR1 &= ~(0x3); // Configure Channel 1 as output

	TIM3->CR1 |= 1;

	TIM3->CCER |= 1;

	// LEFT SIDE
	// Enable TIM4 clock
	TIM4_CLK_EN();
	// Configure PSC, ARR, disable counter, set CNT to 0,
	TIM4->CR1 &= ~(1);

	TIM4->ARR = 999;
	TIM4->PSC = 15;
	TIM4->CNT = 0;

	tempreg = 0;
	tempreg |= (6 << 0x4);
	TIM4->CCMR1 = tempreg;

	TIM4->CCR1 = 0; // 0% duty cycle

	TIM4->CCMR1 &= ~(0x3); // Configure Channel 1 as output

	TIM4->CR1 |= 1;

	TIM4->CCER |= 1;
}

void motor_direction_config(uint8_t dir)
{
	if (dir == FORWARD)
	{
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN1_PIN, 0);
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN2_PIN, 1);
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN3_PIN, 0);
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN4_PIN, 1);
	} else if (dir == BACKWARD)
	{
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN1_PIN, 1);
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN2_PIN, 0);
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN3_PIN, 1);
		GPIO_WriteToOutputPin(MOTOR_DRIVER_IN_PORT, IN4_PIN, 0);
	}
}

void pwm_gpio_init(void)
{
	GPIO_Handle_t handle;
	GPIO_Config_t config;

	// RIGHT GPIO
	handle.pGPIOx = PWM_RIGHT_GPIO_PORT;

	config.GPIO_PinAltFuncMode = 2;
	config.GPIO_PinMode = GPIO_PIN_MODE_AF;
	config.GPIO_PinNumber = PWM_RIGHT_PIN;
	config.GPIO_PinOPType = GPIO_OT_PP;
	config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	config.GPIO_PinSpeed = GPIO_OSPEED_FAST;

	handle.GPIOConfig = config;

	GPIO_Init(&handle);

	// LEFT GPIO
	handle.pGPIOx = PWM_LEFT_GPIO_PORT;
	config.GPIO_PinNumber = PWM_LEFT_PIN;
	handle.GPIOConfig = config;

	GPIO_Init(&handle);


	// Configure motor driver direction pins
	// IN1
	handle.pGPIOx = MOTOR_DRIVER_IN_PORT;

	config.GPIO_PinMode = GPIO_PIN_MODE_OUT;
	config.GPIO_PinNumber = IN1_PIN;
	config.GPIO_PinOPType = GPIO_OT_PP;
	config.GPIO_PinPuPdControl = GPIO_PUPD_NONE;
	config.GPIO_PinSpeed = GPIO_OSPEED_MED;

	handle.GPIOConfig = config;
	GPIO_Init(&handle);

	// IN2
	config.GPIO_PinNumber = IN2_PIN;
	handle.GPIOConfig = config;
	GPIO_Init(&handle);

	// IN3
	config.GPIO_PinNumber = IN3_PIN;
	handle.GPIOConfig = config;
	GPIO_Init(&handle);

	// IN4
	config.GPIO_PinNumber = IN4_PIN;
	handle.GPIOConfig = config;
	GPIO_Init(&handle);


	// Initialize to forward direction
	motor_direction_config(FORWARD);
}

void set_duty_cycle(int32_t duty, TIM_Reg_t* TIM)
{
	if (duty > 0)
	{
		motor_direction_config(FORWARD);
	} else
	{
		motor_direction_config(BACKWARD);
		duty *= -1;
	}

	TIM->CCR1 = duty;
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

	pwm_gpio_init();
	pwm_init();

	us_gpio_inits();

//	lcd_init();

	printf_init(); // For debugging

	while (1)
	{
		uint32_t distance = get_distance();
		printf("distance is %lu \r\n", distance);

		delay(200);
		num_to_str(distance, value);


		if (distance >= 600)
		{
			set_duty_cycle(400, LEFT);
			set_duty_cycle(400, RIGHT);
		} else if (distance >= 200)
		{
			set_duty_cycle(0, LEFT);
			set_duty_cycle(400, RIGHT);
			delay(200);
			set_duty_cycle(0, RIGHT);
			delay(500);
		} else
		{
			set_duty_cycle(-400, LEFT);
			set_duty_cycle(-400, RIGHT);
			delay(1000);
			set_duty_cycle(0, LEFT);
			set_duty_cycle(0, RIGHT);
		}

//		lcd_clear();
//		lcd_write((uint8_t*) distance_str, strlen(distance_str));
//		lcd_write((uint8_t*) value, strlen(value));
	}

	return 0;
}



