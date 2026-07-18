#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include <stdio.h>

extern TIM_HandleTypeDef htim2;

TIM_HandleTypeDef htim4 = {0};

Encoder right_encoder = {.id=1};
Encoder left_encoder = {0};

static void Set_Target_RPM(Encoder* enc);
static void Update_Encoder(Encoder* enc);
static uint32_t Get_Compare(Encoder* enc);

void TIM4_IC_init(void)
{
	TIM_IC_InitTypeDef tim4_ic_config = {0};

	tim4_ic_config.ICFilter = 0;
	tim4_ic_config.ICPolarity = TIM_ICPOLARITY_RISING;
	tim4_ic_config.ICPrescaler = TIM_ICPSC_DIV1;
	tim4_ic_config.ICSelection = TIM_ICSELECTION_DIRECTTI;

	htim4.Init.Period = 62500 - 1;
	htim4.Init.Prescaler = 80 - 1;
	htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim4.Instance = TIM4;

	if (HAL_TIM_IC_Init(&htim4) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_ConfigChannel(&htim4, &tim4_ic_config, RIGHT_ENCA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_Start_IT(&htim4, RIGHT_ENCA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_ConfigChannel(&htim4, &tim4_ic_config, LEFT_ENCA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_Start_IT(&htim4, LEFT_ENCA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	// Base Timer
	if (HAL_TIM_Base_Start_IT(&htim4) != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		// Left Encoder A was triggered
		// If pin B is already on, then its counterclockwise

		// if (HAL_GPIO_ReadPin(RIGHT_ENCB_PORT, RIGHT_ENCB_PIN))
		// {
		// 	right_encoder.ticks--;
		// } else
		// {
		// 	right_encoder.ticks++;
		// }

		right_encoder.ticks++;
	} else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		// Right Encoder A was triggered
		if (HAL_GPIO_ReadPin(LEFT_ENCB_PORT, LEFT_ENCB_PIN))
		{
			left_encoder.ticks++;
		} else
		{
			left_encoder.ticks--;
		}
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM4)
	{
		// Ramp up the target RPM
		Set_Target_RPM(&right_encoder);
		Set_Target_RPM(&left_encoder);
	
		// Update ticks and rpm every 20 ms
		Update_Encoder(&right_encoder);
		Update_Encoder(&left_encoder);

		__HAL_TIM_SET_COMPARE(&htim2, right_encoder.active_pwm_channel, 0);
		__HAL_TIM_SET_COMPARE(&htim2, left_encoder.active_pwm_channel, Get_Compare(&left_encoder));
	}
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

    printf("RPM: %.2f CTarget: %ld offset: %.2f compare: %.2f\r\n", 
        enc->current_rpm,
        enc->target_rpm,
        correction,
		compare / PWM_PERIOD
    );

	return compare;
}

static void Set_Target_RPM(Encoder* enc)
{
	int32_t curr_target = enc->target_rpm;
	int32_t final = enc->final_target_rpm;

	float ramp_percentile = 0.1f;

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
	enc->tick_rate = (enc->ticks - enc->prev_ticks) / S_ELAPSED;
	enc->prev_ticks = enc->ticks;
	enc->current_rpm = (enc->tick_rate * 60) / (TICKS_PER_ROTATION * GEAR_RATIO);
}