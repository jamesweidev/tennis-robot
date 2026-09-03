#include "main.h"
#include "stm32f446xx.h"
#include "stm32f4xx_hal_gpio.h"
#include <stdio.h>

extern Encoder right_encoder;
extern Encoder left_encoder;

RobotState current_state = STATE_IDLE;
uint32_t target_ticks = 0;
uint16_t start_ticks = 0;
ActionType rotate_dir;

volatile float forward_m = 0;
volatile float degs = 0;

static uint16_t Get_Ticks_Traveled(Encoder* enc, uint16_t start_ticks);

// uart testing
void Update_LED(void)
{
	if (forward_m > 1)
	{
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		forward_m = 0;
	}
}

void Update_State(void)
{
	float distance = Get_Distance();

	if (distance == 0.0f)
	{
		distance = 2.0f;
	}

	// if (distance < 0.3 && current_state != OBSTACLE_DETECTED)
	// {
	// 	Stop_Robot();
	// 	current_state = OBSTACLE_DETECTED;
	// }

	switch (current_state)
	{
		case STATE_IDLE:
			// Command receiver will update these values upon reception
            // If there are no commands yet, just keep waiting
            if (forward_m == 0 && degs == 0) {
                break;
            }

			target_ticks = Get_Turn_Ticks(degs);

			// Change rotate direction based on deg sign
			rotate_dir = ACTION_RIGHT;
			if (degs < 0)
			{
				rotate_dir = ACTION_LEFT;
			}

			start_ticks = Get_Ticks(&right_encoder);

            current_state = TURNING_TO_BALL;

			break;

		case TURNING_TO_BALL:
            // Only start turning if its requested at all
            // Otherwise it will drift a bit before stopping
            if (target_ticks != 0){
                Smooth_Drive(rotate_dir);
            }

            // Check if the desired turn is completed
			if (Get_Ticks_Traveled(&right_encoder, start_ticks) >= target_ticks)
			{
				Stop_Robot();
				start_ticks = Get_Ticks(&right_encoder);
				target_ticks = TICKS_PER_METER * forward_m;
				current_state = DRIVING_TO_BALL;

			}

			break;

		case DRIVING_TO_BALL:
            // Prevent drift in the case of 0 forward movement
            if (target_ticks != 0)
            {
                Smooth_Drive(ACTION_FORWARD);
            }

			// Drive completed
			if (Get_Ticks_Traveled(&right_encoder, start_ticks) >= target_ticks)
			{
				Stop_Robot();
				current_state = STATE_IDLE;
				target_ticks = 0;

				// Reset the command
				forward_m = 0;
				degs = 0;
			}

			break;

		case OBSTACLE_DETECTED:
			Smooth_Drive(ACTION_REVERSE);
			if (Get_Distance() > 0.5)
			{
				Stop_Robot();
				current_state = STATE_IDLE;
			}
			break;
	}
}

static uint16_t Get_Ticks_Traveled(Encoder* enc, uint16_t start_ticks)
{
	int16_t delta = Get_Ticks(&right_encoder) - start_ticks;

	return (uint16_t) abs(delta);
}