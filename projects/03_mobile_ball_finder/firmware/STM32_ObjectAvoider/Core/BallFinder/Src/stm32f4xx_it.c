#include "stm32f4xx_it.h"
#include "main.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"


extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;
extern TIM_HandleTypeDef htim7;


void SysTick_Handler(void)
{
  HAL_IncTick();
}

void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart2);
}

void USART6_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart6);
}

void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART6)
    {
      Command_Receive_Callback();
    }
}