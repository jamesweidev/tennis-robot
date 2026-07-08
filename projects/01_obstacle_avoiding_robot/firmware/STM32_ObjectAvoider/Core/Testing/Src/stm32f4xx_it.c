#include "stm32f4xx_it.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"


void SysTick_Handler(void)
{
  HAL_IncTick();
}
