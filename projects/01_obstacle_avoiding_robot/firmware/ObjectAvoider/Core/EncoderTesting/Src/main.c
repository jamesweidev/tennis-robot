#include "main.h"
#include "stm32f4xx_hal_tim.h"


#define PWM_PERIOD 			50000

void TIM2_init(void);
void SystemClock_Config(void);
void Error_Handler(void);

// Motor
void Move_Forward(uint32_t target_rpm);
void TIM3_PWM_Init(void);
void Set_Motor(int32_t pwm_value);


// debug
void GPIO_init(void);
void UART1_init(void);
UART_HandleTypeDef huart1 = {0};

TIM_HandleTypeDef htim2 = {0};

TIM_HandleTypeDef htim3 = {0};
TIM_OC_InitTypeDef pwm_config = {0};

volatile int32_t ticks = 0;
volatile int32_t prev_ticks = 0;
volatile float tick_rate = 0;
volatile float current_rpm = 0;

int32_t target_rpm = 0;
int32_t final_target_rpm = 0;

float prev_err = 0;
float i_value = 0;

volatile int32_t offset = 0;

volatile uint32_t seconds = 0;

char msg[128];

int main(void)
{
	HAL_Init();
	SystemClock_Config(); // 50MHz Sysclk, 50MHz AHB, 25MHz APB1, 50MHz APB2
	
	TIM2_init();
	TIM3_PWM_Init();

	// Debugging
	GPIO_init();
	UART1_init();

	Move_Forward(350);

	while (1)
	{
		sprintf(msg, "RPM diff: %.1f Offset: %ld Seconds: %ld\r\n\n", (target_rpm - current_rpm), offset, seconds);
		HAL_UART_Transmit(&huart1, (uint8_t*) msg, strlen(msg), 100);

		HAL_Delay(1000);
		seconds++;
	}

	return 0;
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	// Encoder A was triggered

	// If pin B is already on, then its counterclockwise
	if (HAL_GPIO_ReadPin(ENCB_PORT, ENCB_PIN))
	{
		ticks--;
	} else
	{
		ticks++;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (target_rpm < final_target_rpm)
	{
		target_rpm += final_target_rpm * 0.2;
	} else
	{
		target_rpm = final_target_rpm;
	}

	float s_elapsed = 0.1f;
	// 20ms elapsed, calculate ticks per second & RPM
	tick_rate = (ticks - prev_ticks) / s_elapsed;
	prev_ticks = ticks;

	current_rpm = (tick_rate * 60) / (13 * 20.409f);


	// Run PID
	float pK = 160;
	float dK = 0;
	float iK = 120;

	float err = target_rpm - current_rpm;

	float d_value = (err - prev_err) / s_elapsed;
	i_value = (i_value + err * s_elapsed);

	offset = pK * err + dK * d_value + iK * i_value;

	prev_err = err;

	Set_Motor(offset);
}	

void Set_Motor(int32_t pwm_value)
{
	uint8_t IN1 = GPIO_PIN_SET;
	uint8_t IN2 = GPIO_PIN_RESET;

	if (pwm_value < 0)
	{
		IN1 = GPIO_PIN_RESET;
		IN2 = GPIO_PIN_SET;

		pwm_value *= -1;
	}

	if (pwm_value > PWM_PERIOD)
	{
		pwm_value = PWM_PERIOD;
	}

	__HAL_TIM_SET_COMPARE(&htim3, PWM_CHANNEL, pwm_value);

	HAL_GPIO_WritePin(IN1_PORT, IN1_PIN, IN1);
	HAL_GPIO_WritePin(IN1_PORT, IN2_PIN, IN2);
}

void Move_Forward(uint32_t rpm_target)
{
	target_rpm = 0;
	final_target_rpm = rpm_target;
}

void TIM2_init(void)
{
	TIM_IC_InitTypeDef tim2_config = {0};

	tim2_config.ICFilter = 0;
	tim2_config.ICPolarity = TIM_ICPOLARITY_RISING;
	tim2_config.ICPrescaler = TIM_ICPSC_DIV1;
	tim2_config.ICSelection = TIM_ICSELECTION_DIRECTTI;

	htim2.Init.Period = 100000;
	htim2.Init.Prescaler = 50 - 1;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Instance = TIM2;

	if (HAL_TIM_IC_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_ConfigChannel(&htim2, &tim2_config, ENCA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_IC_Start_IT(&htim2, ENCA_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	// Base Timer
	if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

}


void TIM3_PWM_Init(void)
{
	htim3.Instance = TIM3;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = PWM_PERIOD - 1;
	htim3.Init.Prescaler = 1 - 1;

	pwm_config.OCMode = TIM_OCMODE_PWM1;
	pwm_config.OCPolarity = TIM_OCPOLARITY_HIGH;
	pwm_config.Pulse = PWM_PERIOD * 0.6f;

	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_ConfigChannel(&htim3, &pwm_config, PWM_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}

	if (HAL_TIM_PWM_Start(&htim3, PWM_CHANNEL) != HAL_OK)
	{
		Error_Handler();
	}
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef osc_init = {0};
    RCC_PLLInitTypeDef pll_init = {0};

    osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    osc_init.HSEState = RCC_HSE_BYPASS;

    pll_init.PLLState = RCC_PLL_ON;
    pll_init.PLLSource = RCC_PLLSOURCE_HSE;
    pll_init.PLLM = 8;
    pll_init.PLLN = 100;
    pll_init.PLLP = 2;
    pll_init.PLLR = 2;
    pll_init.PLLQ = 2;

    osc_init.PLL = pll_init;

    if (HAL_RCC_OscConfig(&osc_init) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitTypeDef clk_init = {0};
    clk_init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_SYSCLK;
    clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clk_init.APB1CLKDivider = RCC_HCLK_DIV1;
    clk_init.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&clk_init, FLASH_LATENCY_1) != HAL_OK)
    {
        Error_Handler();
    }
}


// Debuging

void Error_Handler(void)
{
	while (1);
}

void GPIO_init(void)
{
	// Initialize Onboard User LED for debugging
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef gpio_init = {0};

	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pin = USR_LED_PIN;
	gpio_init.Pull = GPIO_NOPULL;
	gpio_init.Speed = GPIO_SPEED_FREQ_LOW;

	HAL_GPIO_Init(USR_LED_PORT, &gpio_init);
}

void UART1_init(void)
{
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.Mode = UART_MODE_TX;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;

	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}