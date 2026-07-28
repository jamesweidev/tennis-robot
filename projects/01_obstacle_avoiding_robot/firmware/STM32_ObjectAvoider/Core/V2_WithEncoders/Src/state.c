#include "main.h"

extern Encoder right_encoder;

RobotState current_state = STATE_IDLE;
uint32_t target_ticks = 0;
uint32_t start_ticks = 0;
ActionType rotate_dir;

volatile float forward_m = 1;
volatile float degs = 180;

void Update_State(void)
{
	// if (Get_Distance() < 500 && current_state != OBSTACLE_DETECTED)
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

			start_ticks = right_encoder.ticks;

            current_state = TURNING_TO_BALL;
			
			break;

		case TURNING_TO_BALL:
            // Only start turning if its requested at all
            // Otherwise it will drift a bit before stopping
            if (target_ticks != 0){
                Smooth_Drive(rotate_dir);
            }

            // Check if the desired turn is completed
			if (abs(right_encoder.ticks - start_ticks) >= target_ticks)
			{
				Stop_Robot();
				start_ticks = right_encoder.ticks;
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

			if (abs(right_encoder.ticks - start_ticks) >= target_ticks)
			{
				printf("stopped. target ticks: %lu encoder ticks: %ld, start_ticks: %lu\n",
				target_ticks, right_encoder.ticks, start_ticks);
				Stop_Robot();
				current_state = STATE_IDLE;
				target_ticks = 0;
			}

			break;

		case OBSTACLE_DETECTED:
			Smooth_Drive(ACTION_REVERSE);
			if (Get_Distance() > 800)
			{
				current_state = STATE_IDLE;
			}
			break;
	}
}