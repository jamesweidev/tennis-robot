
#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "stm32f4xx_hal.h"

#define PWM_PERIOD              2500

#define US_GPIO_PORT			GPIOA
#define US_TRIG_PIN				GPIO_PIN_8
#define US_ECHO_PIN 			GPIO_PIN_9


// New motor driver pins
#define RIGHT_MOTOR_DRIVER_PORT GPIOA
#define AIN1_PIN				GPIO_PIN_0
#define AIN1_CHANNEL            TIM_CHANNEL_1
#define AIN2_PIN				GPIO_PIN_1
#define AIN2_CHANNEL            TIM_CHANNEL_2

#define LEFT_MOTOR_DRIVER_PORT  GPIOB
#define BIN1_PIN				GPIO_PIN_10
#define BIN1_CHANNEL            TIM_CHANNEL_3
#define BIN2_PIN				GPIO_PIN_2
#define BIN2_CHANNEL            TIM_CHANNEL_4


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

// Encoder constants
#define TICKS_PER_ROTATION      13
#define GEAR_RATIO              20.409f
#define S_ELAPSED               0.1f


#define USART3_TX_PORT          GPIOC
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
    float prev_err;
    float i_value;
} PID_State;

typedef struct {
	volatile int32_t ticks;
	volatile int32_t prev_ticks;
	volatile float tick_rate;
	volatile float current_rpm;
    volatile int32_t target_rpm;
    int32_t final_target_rpm; // used to for smoother motor speed setting
    float starting_rpm;
    PID_State pid;
    uint32_t active_pwm_channel;
    
    // Debugging
    uint8_t id;
} Encoder;

void Drive_Motor(uint32_t rpm, ActionType type);
void motor_direction_config(ActionType type);
float Get_PID_Correction(Encoder* enc);
void Stop_Robot();

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