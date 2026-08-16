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

static void Set_Target_RPM(Encoder* enc);
static void Update_Encoder(Encoder* enc);
static uint32_t Get_Compare(Encoder* enc);

void Encoder_TIM3_TIM4_Init(void)
{
	TIM_Encoder_InitTypeDef encoder_init = {0};
	encoder_init.EncoderMode = TIM_ENCODERMODE_TI12;
	encoder_init.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	encoder_init.IC2Selection = TIM_ICSELECTION_DIRECTTI;

	// Left encoder
    htim3.Instance = TIM3;
    htim3.Init.Period = 0xFFFF;
	htim3.Init.Prescaler = 0;
	left_encoder.htimx = htim3;

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
	right_encoder.htimx = htim4;

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
	// Ramp up the target RPM
	Set_Target_RPM(&right_encoder);
	Set_Target_RPM(&left_encoder);

	// Update ticks and rpm
	Update_Encoder(&right_encoder);
	Update_Encoder(&left_encoder);

	// Avoid setting PWM if target rpm is 0
	// Stop_Robot() already set all channels to 0
	if (right_encoder.final_target_rpm == 0)
	{
		return;
	}

	printf("current rpm: %.2f i value: %.2f actual_s_elapsed: %f\r\n", 
		right_encoder.current_rpm,
		right_encoder.pid.i_value,
		right_encoder.s_elapsed
	);


	// printf("\n\n\n\n\nR RPM %.2f ctarget: %ld \r\n", 
	// 	right_encoder.current_rpm,
	// 	right_encoder.target_rpm
	// );
	// printf("L RPM: %.2f ctarget: %ld \r\n", 
	// 	left_encoder.current_rpm,
	// 	left_encoder.target_rpm
	// );

	__HAL_TIM_SET_COMPARE(&htim2, right_encoder.active_pwm_channel, Get_Compare(&right_encoder));
	__HAL_TIM_SET_COMPARE(&htim2, left_encoder.active_pwm_channel, Get_Compare(&left_encoder));

	// Set the inactive channels to 0 so previous values wont carry over
	__HAL_TIM_SET_COMPARE(&htim2, right_encoder.inactive_pwm_channel, 0);
	__HAL_TIM_SET_COMPARE(&htim2, left_encoder.inactive_pwm_channel, 0);
}

uint16_t Get_Ticks(Encoder* enc)
{
	return (uint16_t)__HAL_TIM_GET_COUNTER(&enc->htimx);
}

static uint32_t Get_Compare(Encoder* enc)
{
	float correction = Get_PID_Correction(enc);
	float BASE_DUTY = 0.04f;

	// For positive rpm
	float duty = correction + BASE_DUTY;

	if (enc->target_rpm < 0)
	{
		duty = BASE_DUTY - correction;
	}

	if (duty > 1) duty = 1;
	if (duty < 0) duty = 0;

	uint32_t compare = (uint32_t) (PWM_PERIOD * duty);


	return compare;
}

static void Set_Target_RPM(Encoder* enc)
{
	int32_t curr_target = enc->target_rpm;
	int32_t final = enc->final_target_rpm;

	float ramp_percentile = 0.05f;

	uint32_t diff = abs(enc->starting_rpm - final);
	// Rather than suddenly setting the speed, it ramps up over (at most) 1 second
	if (abs(curr_target - final) < (diff * ramp_percentile))
	{
		enc->target_rpm = final;
	}else if (curr_target < final)
	{
		enc->target_rpm += diff * ramp_percentile;
	} else if (curr_target > final)
	{
		enc->target_rpm -= diff * ramp_percentile;
	}
}

static void Update_Encoder(Encoder* enc)
{	
	uint32_t cur_millis = HAL_GetTick();
	enc->s_elapsed = (int16_t) (cur_millis - enc->prev_millis) / 1000.0f;
	enc->prev_millis = cur_millis;

	uint16_t ticks = Get_Ticks(enc);
	int16_t tick_rate = (int16_t) (ticks - enc->prev_ticks) / enc->s_elapsed;
	enc->prev_ticks = ticks;
	enc->current_rpm = (tick_rate * 60) / TICKS_PER_ROTATION;
}