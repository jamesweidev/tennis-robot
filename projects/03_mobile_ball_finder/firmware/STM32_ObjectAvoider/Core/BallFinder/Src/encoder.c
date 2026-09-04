#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
#include "stm32f4xx_hal_tim.h"
#include <stdio.h>

extern TIM_HandleTypeDef htim2;

TIM_HandleTypeDef htim3 = {0};
TIM_HandleTypeDef htim4 = {0};
TIM_HandleTypeDef htim7 = {0};

Encoder right_encoder = {0};
Encoder left_encoder = {0};

static void Update_Encoder(Encoder* enc);
static uint32_t Get_Compare(Encoder* enc);
static void Set_Sync_Correction();

void Encoder_TIM3_TIM4_Init(void)
{
	TIM_Encoder_InitTypeDef encoder_init = {0};
	encoder_init.EncoderMode = TIM_ENCODERMODE_TI12;
	encoder_init.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	encoder_init.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	encoder_init.IC1Filter = 2;
	encoder_init.IC2Filter = 2;

	// Left encoder
    htim3.Instance = TIM3;
    htim3.Init.Period = 0xFFFF;
	htim3.Init.Prescaler = 0;
	left_encoder.htimx = &htim3;

    if (HAL_TIM_Encoder_Init(&htim3, &encoder_init) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL)) 
	{
		Error_Handler();
	}
	
	// Right encoder
    htim4.Instance = TIM4;
    htim4.Init.Period = 0xFFFF;
	htim4.Init.Prescaler = 0;
	right_encoder.htimx = &htim4;

    if (HAL_TIM_Encoder_Init(&htim4, &encoder_init) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL)) 
	{
		Error_Handler();
	}
}

void PID_TIM7_Init(void)
{
	htim7.Instance = TIM7;

	// 100 ms period
	htim7.Init.Period = 62500 - 1;
	htim7.Init.Prescaler = 80 - 1;

	if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_Base_Start_IT(&htim7) != HAL_OK)
	{
		Error_Handler();
	}

}

/**
 * @brief Periodically update motor speed based on PID feedback. Runs every 100ms
 * 
 * @param htim 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// Update ticks and rpm
	Update_Encoder(&right_encoder);
	Update_Encoder(&left_encoder);
	
	// Avoid setting PWM in the case of a stopped robot
	// Stop_Robot() already set all channels to 0
	if (right_encoder.final_target_rpm == 0)
	{
		return;
	}

	// Set sync correction to correct for two motors running at slightly different rpm
	Set_Sync_Correction();


	printf("{\"r_rpm\": %.2f, \"r_ctarget\": %ld, \"r_ftarget\": %ld, \"r_starting\": %.2f, \"r_tick_elapsed\": %i,\"l_rpm\": %.2f, \"l_ctarget\": %ld, \"l_ftarget\": %ld, \"l_starting\": %.2f, \"l_tick_elapsed\": %i}\r\n", 
		right_encoder.current_rpm,
		right_encoder.target_rpm,
		right_encoder.final_target_rpm,
		right_encoder.starting_rpm,
		abs(right_encoder.ticks_elapsed),
		left_encoder.current_rpm,
		left_encoder.target_rpm,
		left_encoder.final_target_rpm,
		left_encoder.starting_rpm,
		abs(left_encoder.ticks_elapsed)
	);


	__HAL_TIM_SET_COMPARE(&htim2, right_encoder.active_pwm_channel, Get_Compare(&right_encoder));
	__HAL_TIM_SET_COMPARE(&htim2, left_encoder.active_pwm_channel, Get_Compare(&left_encoder));

	// Set the inactive channels to 0 so previous values wont carry over
	__HAL_TIM_SET_COMPARE(&htim2, right_encoder.inactive_pwm_channel, 0);
	__HAL_TIM_SET_COMPARE(&htim2, left_encoder.inactive_pwm_channel, 0);
}

uint16_t Get_Ticks(Encoder* enc)
{
	return (uint16_t)__HAL_TIM_GET_COUNTER(enc->htimx);
}

static uint32_t Get_Compare(Encoder* enc)
{
	float correction = Get_PID_Correction(enc);
	float BASE_DUTY = 0.1f;

	// Duty for motors to start moving
	float duty = correction + BASE_DUTY;

	if (enc->target_rpm < 0)
	{
		duty = BASE_DUTY - correction;
	}

	// cap duty to the limit values
	if (duty > 1) duty = 1;
	if (duty < 0) duty = 0;

	int32_t compare = (int32_t) (PWM_PERIOD * duty);

	return (uint32_t) compare;
}


static void Update_Encoder(Encoder* enc)
{	
	uint32_t cur_millis = HAL_GetTick();
	enc->s_elapsed = (cur_millis - enc->prev_millis) / 1000.0f;
	enc->prev_millis = cur_millis;

	uint16_t ticks = Get_Ticks(enc);
	int16_t tick_delta = (int16_t) (ticks - enc->prev_ticks);
	enc->ticks_elapsed += tick_delta;
	int16_t tick_rate = tick_delta / enc->s_elapsed;
	enc->prev_ticks = ticks;
	enc->current_rpm = (tick_rate * 60) / TICKS_PER_ROTATION;
}

static int32_t Get_Ticks_Diff()
{
	int32_t r_ticks = abs(right_encoder.ticks_elapsed);
	int32_t l_ticks = abs(left_encoder.ticks_elapsed);

	printf("ticks right: %li left: %li\r\n", r_ticks, l_ticks);

	return (int32_t) (r_ticks - (l_ticks * 0.99f));
}

static void Set_Sync_Correction()
{
	int32_t diff = Get_Ticks_Diff();

	int32_t correction = diff * 0.2f;

	int32_t r_final = right_encoder.final_target_rpm;
	int32_t l_final = left_encoder.final_target_rpm;

	right_encoder.target_rpm = r_final + correction * ((r_final < 0) ? 1 : -1);
	left_encoder.target_rpm = l_final + correction * ((l_final < 0) ? -1 : 1);
}

