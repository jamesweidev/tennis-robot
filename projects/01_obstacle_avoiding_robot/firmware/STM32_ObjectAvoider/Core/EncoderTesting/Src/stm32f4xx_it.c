#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"

extern TIM_HandleTypeDef htim2;

void SysTick_Handler(void)
{
  HAL_IncTick();
}


void TIM2_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim2);
}