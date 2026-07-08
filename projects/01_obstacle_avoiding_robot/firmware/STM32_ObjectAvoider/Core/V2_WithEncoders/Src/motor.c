#include "main.h"

TIM_HandleTypeDef htim2 = {0};

extern Encoder right_encoder;
extern Encoder left_encoder;

void Perform_Action(uint32_t rpm, ActionType type)
{
	motor_direction_config(type);

	right_encoder.final_target_rpm = rpm;
	left_encoder.final_target_rpm = rpm;
}

void TIM2_PWM_Init(void)
{
	TIM_OC_InitTypeDef pwm_init = {0};

	// Period of 20kHz
	htim2.Instance = TIM2;
	htim2.Init.Period = PWM_PERIOD - 1;
	htim2.Init.Prescaler = 1 - 1;

	pwm_init.Pulse = 0;
	pwm_init.OCMode = TIM_OCMODE_PWM1;
	pwm_init.OCPolarity = TIM_OCPOLARITY_HIGH;

	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	// Right PWM config
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, PWMA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim2, PWMA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	// Left PWM config
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, PWMB_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim2, PWMB_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
}

void motor_direction_config(ActionType type)
{
	// Sets motor driver direction pins
	if (type == ACTION_FORWARD)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN2_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN2_PIN, GPIO_PIN_SET);
	} else if (type == ACTION_BACKWARD)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN2_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN2_PIN, GPIO_PIN_RESET);
	} else if (type == ACTION_LEFT)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN2_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN2_PIN, GPIO_PIN_RESET);
	} else if (type == ACTION_RIGHT)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN1_PIN, GPIO_PIN_SET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN2_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN2_PIN, GPIO_PIN_SET);
	} else if (type == ACTION_STOP)
	{
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, AIN2_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN1_PIN, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(MOTOR_DRIVER_IN_PORT, BIN2_PIN, GPIO_PIN_RESET);
	}
}