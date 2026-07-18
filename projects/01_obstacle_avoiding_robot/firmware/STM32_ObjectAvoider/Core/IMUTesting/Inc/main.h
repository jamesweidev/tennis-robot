
#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f446xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define IMU_I2C_PORT        GPIOB
#define IMU_SCL_PIN         GPIO_PIN_6
#define IMU_SDL_PIN         GPIO_PIN_7

#define DEBUG_UART_TX_PIN   GPIO_PIN_9
#define DEBUG_UART_TX_PORT  GPIOA

#endif