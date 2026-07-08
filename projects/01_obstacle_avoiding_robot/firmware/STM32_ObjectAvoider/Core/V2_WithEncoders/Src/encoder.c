#include "main.h"
#include "stm32f4xx_hal_tim.h"

extern TIM_HandleTypeDef htim2;

TIM_HandleTypeDef htim4 = {0};

Encoder right_encoder = {0};
Encoder left_encoder = {0};

static void Set_Target_RPM(Encoder* enc);
static void Update_Encoder(Encoder* enc, float s_elapsed);

void TIM4_IC_init(void)
{
	TIM_IC_InitTypeDef tim4_ic_config = {0};

	tim4_ic_config.ICFilter = 0;
	tim4_ic_config.ICPolarity = TIM_ICPOLARITY_RISING;
	tim4_ic_config.ICPrescaler = TIM_ICPSC_DIV1;
	tim4_ic_config.ICSelection = TIM_ICSELECTION_DIRECTTI;

	htim4.Init.Period = 100000;
	htim4.Init.Prescaler = 50 - 1;
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
		if (HAL_GPIO_ReadPin(RIGHT_ENCB_PORT, RIGHT_ENCB_PIN))
		{
			right_encoder.ticks--;
		} else
		{
			right_encoder.ticks--;
		}
	} else if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		// Right Encoder A was triggered
		if (HAL_GPIO_ReadPin(LEFT_ENCB_PORT, LEFT_ENCB_PIN))
		{
			left_encoder.ticks--;
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
		Set_Target_RPM(&right_encoder);
		Set_Target_RPM(&left_encoder);
	
		// update ticks and rpm every 20 ms
		float s_elapsed = 0.2f;
		Update_Encoder(&right_encoder, s_elapsed);
		Update_Encoder(&left_encoder, s_elapsed);

        int32_t right_offset = Get_PID_Offset(right_encoder.target_rpm, right_encoder.current_rpm, s_elapsed);
        int32_t left_offset = Get_PID_Offset(left_encoder.target_rpm, left_encoder.current_rpm, s_elapsed);

	
        if (right_offset < 0)
        {
            right_offset *= -1;
        }
        if (left_offset < 0)
        {
            left_offset *= -1;
        }
	
		__HAL_TIM_SET_COMPARE(&htim2, PWMA_CHANNEL, right_offset);
		__HAL_TIM_SET_COMPARE(&htim2, PWMB_CHANNEL, left_offset);
	}
}	

static void Set_Target_RPM(Encoder* enc)
{
	int32_t target = enc->target_rpm;
	int32_t final = enc->final_target_rpm;

	// Rather than suddenly setting the speed, it ramps up over (maximum) 1 second
	if (abs(target - final) < (final * 0.2))
	{
		target = final;
	}else if (target < final)
	{
		target += final * 0.2;
	} else if (target > final)
	{
		target -= final * 0.2;
	}
}

static void Update_Encoder(Encoder* enc, float s_elapsed)
{
	enc->tick_rate = (enc->ticks - enc->prev_ticks) / s_elapsed;
	enc->prev_ticks = enc->ticks;
	enc->current_rpm = (enc->tick_rate * 60) / (13 * 20.409f);
}