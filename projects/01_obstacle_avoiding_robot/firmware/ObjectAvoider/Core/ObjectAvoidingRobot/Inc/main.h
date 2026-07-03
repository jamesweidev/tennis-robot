
#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"

#define PWM_PERIOD              2500

#define US_GPIO_PORT			GPIOA
#define US_TRIG_PIN				GPIO_PIN_8
#define US_ECHO_PIN 			GPIO_PIN_9

#define PWM_RIGHT_GPIO_PORT		GPIOA
#define PWM_RIGHT_PIN			GPIO_PIN_0

#define PWM_LEFT_GPIO_PORT		GPIOA
#define PWM_LEFT_PIN			GPIO_PIN_1

#define MOTOR_DRIVER_IN_PORT	GPIOA

#define IN1_PIN					GPIO_PIN_6
#define IN2_PIN					GPIO_PIN_7
#define IN3_PIN					GPIO_PIN_4
#define IN4_PIN					GPIO_PIN_5

#define USART1_TX_PORT          GPIOB
#define USART1_TX_PIN           GPIO_PIN_6

// Directions
typedef enum
{
    ACTION_FORWARD,
    ACTION_BACKWARD,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_STOP
} ActionType;

#endif