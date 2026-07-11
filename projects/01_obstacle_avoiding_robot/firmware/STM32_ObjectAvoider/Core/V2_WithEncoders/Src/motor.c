#include "main.h"

TIM_HandleTypeDef htim2 = {0};

extern Encoder right_encoder;
extern Encoder left_encoder;

void Drive_Motor(uint32_t rpm, ActionType type)
{
	// Completely reset both encoders when rpm is set to 0
	// 
	if (rpm == 0)
	{
		right_encoder = (Encoder) {.id=1};
		left_encoder = (Encoder) {0};
		return;
	}

	right_encoder.starting_rpm = right_encoder.current_rpm;
	left_encoder.starting_rpm = left_encoder.current_rpm;

	// Adjust the sign of rpm based on the specified direction
	// Defaults to positive, changes to negative if needed
	uint32_t r_rpm = rpm;
	uint32_t l_rpm = rpm;
	if (type == ACTION_RIGHT || type == ACTION_BACKWARD)
	{
		r_rpm = -rpm;
	}
	if (type == ACTION_LEFT || type == ACTION_BACKWARD)
	{
		l_rpm = -rpm;
	}

	// New rpm is different from previous. Otherwise no reason to reset i value
	if (right_encoder.final_target_rpm != r_rpm)
	{
		// Reset the PID i Value so it doesn't bleed into the next rpm
		right_encoder.pid.i_value = 0;

		// The base duty cycle is 10%, which results in ~120 rpm
		// So start the target rpm at 150 so PID doesnt overshoot
		// Or if rpm is less than rpm start at rpm
		right_encoder.target_rpm = (rpm < 150) ? rpm : 0;
	}
	// Same for the left motor
	if (left_encoder.final_target_rpm != l_rpm)
	{
		left_encoder.pid.i_value = 0;

		left_encoder.target_rpm = (rpm < 150) ? rpm : 0;
	}

	// Set the final RPM
	right_encoder.final_target_rpm = r_rpm;
	left_encoder.final_target_rpm = l_rpm;

	motor_direction_config(type);
}

void Stop_Robot()
{
	Drive_Motor(0, ACTION_STOP);
}

void TIM2_PWM_Init(void)
{
	TIM_OC_InitTypeDef pwm_init = {0};

	// 20kHz Frequency
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