
#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"

#define PWM_PERIOD              50000

#define US_GPIO_PORT			GPIOA
#define US_TRIG_PIN				GPIO_PIN_8
#define US_ECHO_PIN 			GPIO_PIN_9

#define PWMA_PORT		        GPIOA
#define PWMA_PIN			    GPIO_PIN_0
#define PWMA_CHANNEL            TIM_CHANNEL_1

#define PWMB_PORT		        GPIOA
#define PWMB_PIN			    GPIO_PIN_1
#define PWMB_CHANNEL            TIM_CHANNEL_2

#define MOTOR_DRIVER_IN_PORT	GPIOA
#define AIN1_PIN				GPIO_PIN_6
#define AIN2_PIN				GPIO_PIN_7
#define BIN1_PIN				GPIO_PIN_4
#define BIN2_PIN				GPIO_PIN_5

// Encoder IC is on TIM4
#define RIGHT_ENCA_PORT         GPIOB
#define RIGHT_ENCA_PIN          GPIO_PIN_6
#define RIGHT_ENCA_CHANNEL      TIM_CHANNEL_1

#define RIGHT_ENCB_PORT         GPIOB
#define RIGHT_ENCB_PIN          GPIO_PIN_7
#define RIGHT_ENCB_CHANNEL      TIM_CHANNEL_2

#define LEFT_ENCA_PORT          GPIOB
#define LEFT_ENCA_PIN           GPIO_PIN_8
#define LEFT_ENCA_CHANNEL       TIM_CHANNEL_3

#define LEFT_ENCB_PORT          GPIOB
#define LEFT_ENCB_PIN           GPIO_PIN_9
#define LEFT_ENCB_CHANNEL       TIM_CHANNEL_4

#define USART3_TX_PORT          GPIOB
#define USART3_TX_PIN           GPIO_PIN_10

// Directions
typedef enum
{
    ACTION_FORWARD,
    ACTION_BACKWARD,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_STOP
} ActionType;

typedef struct {
	volatile int32_t ticks;
	volatile int32_t prev_ticks;
	volatile float tick_rate;
	volatile float current_rpm;
    volatile int32_t target_rpm;
    int32_t final_target_rpm; // used to for smoother motor speed setting
} Encoder;

void Perform_Action(uint32_t speed, ActionType type);
void motor_direction_config(ActionType type);
int32_t Get_PID_Offset(int32_t target_rpm, float current_rpm, float s_elapsed);

// ultrasonic
uint32_t get_distance(void);
uint32_t micros(void);
void delay_us(uint32_t micros);

// Inits
void Ultrasonic_GPIO_Init(void);
void TIM4_IC_init(void);
void TIM3_Micros_Init(void);
void TIM2_PWM_Init(void);
void SystemClock_Config(void);

// debugging
void UART3_Init(void);
void Error_Handler(void);

#endif