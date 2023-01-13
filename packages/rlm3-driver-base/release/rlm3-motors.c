#include "rlm3-motors.h"
#include "rlm3-gpio.h"
#include "tim.h"
#include "main.h"
#include "Assert.h"


static void StopMotors()
{
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
	__HAL_TIM_SET_COMPARE(&htim13, TIM_CHANNEL_1, 0);
	__HAL_TIM_SET_COMPARE(&htim14, TIM_CHANNEL_1, 0);
}

extern void RLM3_Motors_Init()
{
	ASSERT(RLM3_GPIO_IsInit());
	ASSERT(!RLM3_Motors_IsInit());

	MX_TIM1_Init();
	MX_TIM3_Init();
	MX_TIM13_Init();
	MX_TIM14_Init();

	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start(&htim13);
	HAL_TIM_Base_Start(&htim14);

	StopMotors();

	HAL_TIM_OC_Start(&htim3,  TIM_CHANNEL_3);
	HAL_TIM_OC_Start(&htim3,  TIM_CHANNEL_4);
	HAL_TIM_OC_Start(&htim1,  TIM_CHANNEL_2);
	HAL_TIM_OC_Start(&htim1,  TIM_CHANNEL_3);
	HAL_TIM_OC_Start(&htim13, TIM_CHANNEL_1);
	HAL_TIM_OC_Start(&htim14, TIM_CHANNEL_1);
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

	StopMotors();
	RLM3_GPIO_MotorEnable_SetLow();
}

extern void RLM3_Motors_Disable()
{
	ASSERT(RLM3_Motors_IsEnabled());

	StopMotors();
	RLM3_GPIO_MotorEnable_SetHigh();
}

extern bool RLM3_Motors_IsEnabled()
{
	return RLM3_GPIO_MotorEnable_IsLow();
}

static void SetDuty(TIM_HandleTypeDef* timer, uint32_t channel, uint32_t period, float duty)
{
	ASSERT(duty >= 0.0f && duty <= 1.0f);
	uint32_t iduty = (uint32_t)((period + 1) * duty);
	__HAL_TIM_SET_COMPARE(timer, channel, iduty);
}

static inline void SetHBridge(TIM_HandleTypeDef* a_timer, uint32_t a_channel, TIM_HandleTypeDef* b_timer, uint32_t b_channel, uint32_t period, float duty)
{
	ASSERT(-1.0f <= duty && duty <= 1.0f);

	if (-1.0f <= duty && duty < 0.0f)
	{
		__HAL_TIM_SET_COMPARE(a_timer, a_channel, 0);
		SetDuty(b_timer, b_channel, period, -duty);
	}
	else if (0.0f < duty && duty <= 1.0f)
	{
		__HAL_TIM_SET_COMPARE(b_timer, b_channel, 0);
		SetDuty(a_timer, a_channel, period, duty);
	}
	else
	{
		__HAL_TIM_SET_COMPARE(a_timer, a_channel, 0);
		__HAL_TIM_SET_COMPARE(b_timer, b_channel, 0);
	}
}

extern void RLM3_Motors_SetWheels(float left, float right)
{
	ASSERT(RLM3_Motors_IsEnabled());

	SetHBridge(&htim3, TIM_CHANNEL_4, &htim3, TIM_CHANNEL_3, 9000, left);
	SetHBridge(&htim1, TIM_CHANNEL_2, &htim1, TIM_CHANNEL_3, 4500, right);
}

extern void RLM3_Motors_SetBlade(float blade)
{
	ASSERT(RLM3_Motors_IsEnabled());

	SetHBridge(&htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1, 4500, blade);
}
