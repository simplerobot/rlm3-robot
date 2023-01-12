#include "rlm3-motors.h"
#include "tim.h"
#include "main.h"
#include "Assert.h"


extern void RLM3_Motors_Init()
{
	ASSERT(!RLM3_Motors_IsInit());

	MX_TIM1_Init();
	MX_TIM3_Init();
	MX_TIM13_Init();
	MX_TIM14_Init();
}

extern void RLM3_Motors_DeInit()
{
	ASSERT(RLM3_Motors_IsInit());

	HAL_TIM_Base_DeInit(&htim1);
	HAL_TIM_Base_DeInit(&htim3);
	HAL_TIM_Base_DeInit(&htim13);
	HAL_TIM_Base_DeInit(&htim14);
}

extern bool RLM3_Motors_IsInit()
{
	return __HAL_RCC_TIM1_IS_CLK_ENABLED();
}

extern void RLM3_Motors_Enable()
{
	ASSERT(RLM3_Motors_IsInit());
	ASSERT(!RLM3_Motors_IsEnabled());

	HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port, MOTOR_ENABLE_Pin, GPIO_PIN_RESET);
}

extern void RLM3_Motors_Disable()
{
	ASSERT(RLM3_Motors_IsEnabled());

	HAL_GPIO_WritePin(MOTOR_ENABLE_GPIO_Port, MOTOR_ENABLE_Pin, GPIO_PIN_SET);
}

extern bool RLM3_Motors_IsEnabled()
{
	if (!RLM3_Motors_IsInit())
		return false;
	return (HAL_GPIO_ReadPin(MOTOR_ENABLE_GPIO_Port, MOTOR_ENABLE_Pin) != GPIO_PIN_RESET);
}

static inline void SetHBridge(TIM_HandleTypeDef* a_timer, uint32_t a_channel, TIM_HandleTypeDef* b_timer, uint32_t b_channel, uint32_t period, int8_t duty)
{
	if (duty > 0)
	{
		__HAL_TIM_SET_COMPARE(b_timer, b_channel, 0);
		__HAL_TIM_SET_COMPARE(a_timer, a_channel, (1 + duty) * (period + 1) / 128);
	}
	else if (duty < -1)
	{
		__HAL_TIM_SET_COMPARE(a_timer, a_channel, 0);
		__HAL_TIM_SET_COMPARE(b_timer, b_channel, (0 - duty) * (period + 1) / 128);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(a_timer, a_channel, 0);
		__HAL_TIM_SET_COMPARE(b_timer, b_channel, 0);
	}
}

extern void RLM3_Motors_SetWheels(int8_t left, int8_t right)
{
	ASSERT(RLM3_Motors_IsEnabled);

	SetHBridge(&htim3, TIM_CHANNEL_4, &htim3, TIM_CHANNEL_3, 9000, left);
	SetHBridge(&htim1, TIM_CHANNEL_2, &htim1, TIM_CHANNEL_3, 4500, right);
}

extern void RLM3_Motors_SetBlade(int8_t blade)
{
	ASSERT(RLM3_Motors_IsEnabled);

	SetHBridge(&htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1, 4500, blade);
}
