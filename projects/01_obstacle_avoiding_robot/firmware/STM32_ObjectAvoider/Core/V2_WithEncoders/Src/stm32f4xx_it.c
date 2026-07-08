#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"


extern UART_HandleTypeDef huart3;
extern TIM_HandleTypeDef htim4;


void SysTick_Handler(void)
{
  HAL_IncTick();
}

void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart3);
}

void TIM4_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim4);
}