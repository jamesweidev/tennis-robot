#include "main.h"
#include "stm32f4xx_hal.h"

TIM_HandleTypeDef htim2 = {0};

extern Encoder right_encoder;
extern Encoder left_encoder;

static void Drive_Motor(int32_t r_rpm, int32_t l_rpm, ActionType type);
static void motor_direction_config(ActionType type);

ActionType current_action = ACTION_STOP;


// Calculate relevant constants outside of function so it doesn't run repeatedly
// Wheels are 48mm in diameter
float ticks_per_wheel_rotation = TICKS_PER_ROTATION * GEAR_RATIO;
float wheel_circumference = 3.14 * 0.048;

/**
 * @brief Turn clockwise_degs degrees clockwise, then go forward by forward_m meters
 * 
 * @param forward_m meters to move forward by
 * @param degs degrees to turn by. positive for clockwise, and vice versa
 */
void Offset_Position(float forward_m, float degs)
{
	// Stop the robot in case it wasn't already
	Stop_Robot();

	float ticks_per_m = ticks_per_wheel_rotation / wheel_circumference;

	// From the center of one wheel to the other, 
	// basically the diameter of the rotation
	float wheel_to_wheel_m = 0.14;

	// Distance both wheels each have to travel to achieve a 180 deg rotation
	// which is just half the circumference
	float distance_180_deg = wheel_to_wheel_m * 3.14f / 2;

	// The distance both wheels actually have to travel in meters
	float m_to_travel = distance_180_deg * degs / 180;

	// in ticks
	uint32_t rotate_ticks = abs((uint32_t) (m_to_travel * ticks_per_m));

	// Change rotate direction based on deg sign
	ActionType rotate_dir = ACTION_RIGHT;
	Encoder *enc = &right_encoder;
	if (degs < 0)
	{
		rotate_dir = ACTION_LEFT;
		enc = &left_encoder;
	}

	// Wait until the desired turn is completed
	uint32_t start_ticks = enc->ticks;
	Smooth_Drive(rotate_dir);
	while (enc->ticks - start_ticks < rotate_ticks);

	uint32_t ticks_offset = ticks_per_m * forward_m;
	start_ticks = right_encoder.ticks;

	// Go forward until the distance has been traversed
	Smooth_Drive(ACTION_FORWARD);
	while (right_encoder.ticks - start_ticks < ticks_offset);
	Stop_Robot();
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
	if (current_action == ACTION_STOP)
	{
		Drive_Motor(300, 300, new_action);
		current_action = new_action;
		return;
	}

	if (current_action != new_action)
	{
		// If the new action differs from current action
		// Stop the robot for 800ms first to prevent jitters
		Stop_Robot();
		HAL_Delay(800);
		Drive_Motor(300, 300, new_action);
		current_action = new_action;
	}
}

static void Drive_Motor(int32_t r_rpm, int32_t l_rpm, ActionType type)
{
	right_encoder.starting_rpm = right_encoder.current_rpm;
	left_encoder.starting_rpm = left_encoder.current_rpm;

	// Adjust the sign of rpm based on the specified direction
	// Defaults to positive, changes to negative if needed
	if (type == ACTION_RIGHT || type == ACTION_BACKWARD)
	{
		r_rpm *= -1;
	}
	if (type == ACTION_LEFT || type == ACTION_BACKWARD)
	{
		l_rpm *= -1;
	}

	// reset i value if new rpm differs from previous. 
	if (right_encoder.final_target_rpm != r_rpm)
	{
		// Reset the PID i Value so it doesn't bleed into the next rpm
		right_encoder.pid.i_value = 0;

		right_encoder.target_rpm = 0;
	}
	// Same for the left motor
	if (left_encoder.final_target_rpm != l_rpm)
	{
		left_encoder.pid.i_value = 0;

		left_encoder.target_rpm = 0;
	}

	// Set the final RPM target
	right_encoder.final_target_rpm = r_rpm;
	left_encoder.final_target_rpm = l_rpm;

	motor_direction_config(type);
}

void Stop_Robot()
{
	// Rest all encoder values so they don't bleed into future speed settings
	right_encoder = (Encoder) {.id=1};
	left_encoder = (Encoder) {0};

	// Set all motor driver IN pins to a logical 1, making motors coast

	// Setting all to max PWM shorts it to ground, but also makes it jerk
	// Since not all channels can be set exactly at once
	__HAL_TIM_SET_COMPARE(&htim2, AIN1_CHANNEL, 0);
	__HAL_TIM_SET_COMPARE(&htim2, AIN2_CHANNEL, 0);
	__HAL_TIM_SET_COMPARE(&htim2, BIN1_CHANNEL, 0);
	__HAL_TIM_SET_COMPARE(&htim2, BIN2_CHANNEL, 0);
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
		right_encoder.active_pwm_channel = AIN2_CHANNEL;
		left_encoder.active_pwm_channel = BIN1_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN1_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN2_CHANNEL;
	} else if (type == ACTION_BACKWARD)
	{
		right_encoder.active_pwm_channel = AIN1_CHANNEL;
		left_encoder.active_pwm_channel = BIN2_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN2_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN1_CHANNEL;
	} else if (type == ACTION_LEFT)
	{
		right_encoder.active_pwm_channel = AIN2_CHANNEL;
		left_encoder.active_pwm_channel = BIN2_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN1_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN1_CHANNEL;
	} else if (type == ACTION_RIGHT)
	{
		right_encoder.active_pwm_channel = AIN1_CHANNEL;
		left_encoder.active_pwm_channel = BIN1_CHANNEL;

		right_encoder.inactive_pwm_channel = AIN2_CHANNEL;
		left_encoder.inactive_pwm_channel = BIN2_CHANNEL;
	}
}