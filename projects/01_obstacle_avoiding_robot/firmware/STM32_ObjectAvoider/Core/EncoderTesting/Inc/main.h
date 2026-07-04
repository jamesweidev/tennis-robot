
#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define IN1_PORT            GPIOA
#define IN1_PIN             GPIO_PIN_8

#define IN2_PORT            GPIOA
#define IN2_PIN             GPIO_PIN_10

#define PWM_CHANNEL         TIM_CHANNEL_1

#define ENCA_PORT           GPIOA
#define ENCA_PIN            GPIO_PIN_0
#define ENCA_CHANNEL        TIM_CHANNEL_1

#define ENCB_PORT           GPIOA
#define ENCB_PIN            GPIO_PIN_1

#define USR_LED_PORT        GPIOA
#define USR_LED_PIN         GPIO_PIN_5

#define PWM_PORT            GPIOA
#define PWM_PIN             GPIO_PIN_6

#define DEBUG_UART_TX_PIN   GPIO_PIN_9
#define DEBUG_UART_TX_PORT  GPIOA

#endif