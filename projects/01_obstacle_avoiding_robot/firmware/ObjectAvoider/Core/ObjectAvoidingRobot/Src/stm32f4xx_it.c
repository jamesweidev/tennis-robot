#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_uart.h"


extern UART_HandleTypeDef huart1;


void SysTick_Handler(void)
{
  HAL_IncTick();
}

void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}