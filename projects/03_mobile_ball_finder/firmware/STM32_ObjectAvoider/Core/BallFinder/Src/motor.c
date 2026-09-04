#include "main.h"
#include "stm32f4xx_hal.h"

TIM_HandleTypeDef htim2 = {0};

extern Encoder right_encoder;
extern Encoder left_encoder;

static void Drive_Motor(int32_t r_rpm, int32_t l_rpm, ActionType type);
static void motor_direction_config(ActionType type);

ActionType current_action = ACTION_STOP;

/**
 * @brief Returns the number of motor ticks it would take to rotate degs degrees
 * 
 * @param degs 
 */
uint32_t Get_Turn_Ticks(float degs)
{
	// Distance both wheels each have to travel to achieve a 180 deg rotation
	// which is just half the circumference
	float distance_180_deg = WHEEL_DISTANCE_M * 3.14f / 2;

	// The distance both wheels actually have to travel in meters
	// degs could be negative, but direction is already taken care of in state machine
	float m_to_travel = fabsf(distance_180_deg * degs / 180);

	// in ticks
	uint32_t rotate_ticks = (uint32_t) (m_to_travel * TICKS_PER_METER);

	// correction
	rotate_ticks *= 0.98f;

	return rotate_ticks;
}


void Draw_Circle(float radius)
{
	float wheel_to_wheel_m = 0.14;
	uint32_t high_rpm = 400;
	uint32_t low_rpm = high_rpm * (1 - wheel_to_wheel_m / radius);

	Drive_Motor(high_rpm, low_rpm, ACTION_FORWARD);
}

void Smooth_Drive(ActionType new_action)
{
	#define RPM 100

	if (current_action == ACTION_STOP)
	{
		Drive_Motor(RPM, RPM, new_action);
		current_action = new_action;
		return;
	}

	if (current_action != new_action)
	{
		// If the new action differs from current action
		// Stop the robot for some time first to prevent jitters
		Stop_Robot();
		HAL_Delay(1000);
		Drive_Motor(RPM, RPM, new_action);
		current_action = new_action;
	}
}

static void Drive_Motor(int32_t r_rpm, int32_t l_rpm, ActionType type)
{
	right_encoder.starting_rpm = right_encoder.current_rpm;
	left_encoder.starting_rpm = left_encoder.current_rpm;

	right_encoder.ticks_elapsed = 0;
	left_encoder.ticks_elapsed = 0;

    right_encoder.prev_millis = HAL_GetTick();
    left_encoder.prev_millis = HAL_GetTick();

	// Adjust the sign of rpm based on the specified direction
	if (type == ACTION_RIGHT || type == ACTION_REVERSE)
	{
		r_rpm *= -1;
	}
	if (type == ACTION_LEFT || type == ACTION_REVERSE)
	{
		l_rpm *= -1;
	}

	// reset i value if new rpm differs from previous. 
	if (right_encoder.final_target_rpm != r_rpm)
	{
		// Reset the PID i Value so it doesn't bleed into the next rpm
		right_encoder.pid.i_value = 0;
	}
	// Same for the left motor
	if (left_encoder.final_target_rpm != l_rpm)
	{
		left_encoder.pid.i_value = 0;
	}

	// Set the final RPM target
	right_encoder.final_target_rpm = r_rpm;
	left_encoder.final_target_rpm = l_rpm;

	// Initialize the target to the current rpm
	right_encoder.target_rpm = r_rpm;
	left_encoder.target_rpm = l_rpm;

	motor_direction_config(type);
}

void Stop_Robot()
{
	// Set to 0 for 0 speed
	__HAL_TIM_SET_COMPARE(&htim2, AIN1_CHANNEL, PWM_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim2, AIN2_CHANNEL, PWM_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim2, BIN1_CHANNEL, PWM_PERIOD);
	__HAL_TIM_SET_COMPARE(&htim2, BIN2_CHANNEL, PWM_PERIOD);

	// Rest all encoder values so they don't bleed into future speed settings
	right_encoder = (Encoder) {
		.htimx = right_encoder.htimx, 
		.prev_ticks = Get_Ticks(&right_encoder)
	};
	left_encoder = (Encoder) {
		.htimx = left_encoder.htimx, 
		.prev_ticks = Get_Ticks(&left_encoder)
	};

	current_action = ACTION_STOP;
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
	// AIN1
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, AIN1_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Start(&htim2, AIN1_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
	// AIN2
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, AIN2_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Start(&htim2, AIN2_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	__HAL_TIM_SET_COMPARE(&htim2, AIN1_CHANNEL, 0);
	__HAL_TIM_SET_COMPARE(&htim2, AIN2_CHANNEL, 0);

	// Left PWM config
	// BIN1
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, BIN1_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Start(&htim2, BIN1_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
	// BIN2
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &pwm_init, BIN2_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Start(&htim2, BIN2_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
}

static void motor_direction_config(ActionType type)
{
	// Sets motor driver direction pins
	if (type == ACTION_FORWARD)
	{
		right_encoder.active_pwm_channel = AIN1_CHANNEL;
		left_encoder.active_pwm_channel = BIN1_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN2_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN2_CHANNEL;
	} else if (type == ACTION_REVERSE)
	{
		right_encoder.active_pwm_channel = AIN2_CHANNEL;
		left_encoder.active_pwm_channel = BIN2_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN1_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN1_CHANNEL;
	} else if (type == ACTION_LEFT)
	{
		right_encoder.active_pwm_channel = AIN1_CHANNEL;
		left_encoder.active_pwm_channel = BIN2_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN2_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN1_CHANNEL;
	} else if (type == ACTION_RIGHT)
	{
		right_encoder.active_pwm_channel = AIN2_CHANNEL;
		left_encoder.active_pwm_channel = BIN1_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN1_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN2_CHANNEL;
	}
}