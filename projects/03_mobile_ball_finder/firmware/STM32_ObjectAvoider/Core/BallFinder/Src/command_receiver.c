#include "main.h"
#include "stm32f4xx_hal_uart.h"
#include <stdlib.h>
#include <string.h>


UART_HandleTypeDef huart6 = {0};

extern float forward_m;
extern float degs;

char cmd_str[32];

void Receive_Command(void)
{
    // Receive command in the format: "f: xx.xx d: xxx.xx"
    HAL_UARTEx_ReceiveToIdle_IT(&huart6, (uint8_t*) cmd_str, sizeof(cmd_str));
}

void Command_Receive_Callback(void)
{
    // split cmd_str by space, and get the second and third value
    char* token = strtok(cmd_str, " "); // "f:"
    char* forward_str = strtok(NULL, " ");
    token = strtok(NULL, " "); // "d:"
    char* degs_str = strtok(NULL, " ");

    (void) token;

    char *endptr;

    // convert string to float
    forward_m = strtof(forward_str, &endptr);
    degs = strtof(degs_str, &endptr);

    Receive_Command();
}

void Command_UART6_Init(void)
{
    huart6.Instance = USART6;

    // defaults to 8b word, 1b stop, no parity, no hw flow control, 16 oversampling
    huart6.Init.BaudRate = 115200;
    huart6.Init.Mode = UART_MODE_TX_RX;

    HAL_UART_Init(&huart6);
}