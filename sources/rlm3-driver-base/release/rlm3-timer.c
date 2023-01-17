#include "rlm3-timer.h"
#include "main.h"


static TIM_HandleTypeDef g_htim2 = { 0 };


extern void RLM3_Timer2_Init(size_t frequency_hz)
{
	__HAL_RCC_TIM2_CLK_ENABLE();

	g_htim2.Instance = TIM2;
	g_htim2.Init.Prescaler = 0;
	g_htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	g_htim2.Init.Period = 180000000 / frequency_hz / 2;
	g_htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	g_htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&g_htim2) != HAL_OK)
	{
		Error_Handler();
	}

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&g_htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&g_htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);
	if (HAL_TIM_Base_Start_IT(&g_htim2) != HAL_OK)
	{
		Error_Handler();
	}

//	__HAL_TIM_ENABLE_IT(&g_htim2, TIM_IT_UPDATE);
}

extern void RLM3_Timer2_Deinit()
{
//	__HAL_TIM_DISABLE_IT(&g_htim2, TIM_IT_UPDATE);

	HAL_TIM_Base_Stop_IT(&g_htim2);
	HAL_TIM_Base_DeInit(&g_htim2);
	__HAL_RCC_TIM2_CLK_DISABLE();
}

extern bool RLM3_Timer2_IsInit()
{
	return __HAL_RCC_TIM2_IS_CLK_ENABLED();
}

extern __attribute__((weak)) void RLM3_Timer2_Event_CB_ISR()
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
}

extern void TIM2_IRQHandler(void)
{
	__HAL_TIM_CLEAR_IT(&g_htim2, TIM_IT_UPDATE);
	RLM3_Timer2_Event_CB_ISR();
}
