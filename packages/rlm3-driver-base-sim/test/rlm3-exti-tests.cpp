#include "Test.hpp"
#include "rlm3-gpio.h"
#include "rlm3-task.h"
#include "rlm3-sim.hpp"


size_t g_exti_count = 0;
RLM3_Task g_client_task = NULL;

extern void RLM3_EXTI12_Callback()
{
	g_exti_count++;
	RLM3_GiveFromISR(g_client_task);
}

TEST_CASE(SIM_GPIO_Interrupt_HappyCase)
{
	g_client_task = RLM3_GetCurrentTask();
	g_exti_count = 0;
	SIM_AddDelay(10);
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init = {};
	init.Pin = GPIO_PIN_12;
	init.Mode = GPIO_MODE_IT_RISING;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &init);

	ASSERT(g_exti_count == 0);
	RLM3_Take();
	ASSERT(g_exti_count == 1);
	ASSERT(RLM3_GetCurrentTime() == 10);
}

TEST_CASE(SIM_GPIO_Interrupt_ClockNotEnabled)
{
	g_client_task = RLM3_GetCurrentTask();
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);
	SIM_AddInterrupt([&]() { RLM3_GiveFromISR(g_client_task); });

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init = {};
	init.Pin = GPIO_PIN_12;
	init.Mode = GPIO_MODE_IT_RISING;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &init);
	__HAL_RCC_GPIOA_CLK_DISABLE();

	ASSERT_ASSERTS(RLM3_Take());
	RLM3_Take();
}

TEST_CASE(SIM_GPIO_Interrupt_PinNotEnabled)
{
	g_client_task = RLM3_GetCurrentTask();
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);
	SIM_AddInterrupt([&]() { RLM3_GiveFromISR(g_client_task); });

	__HAL_RCC_GPIOA_CLK_ENABLE();

	ASSERT_ASSERTS(RLM3_Take());
	RLM3_Take();
}

TEST_CASE(SIM_GPIO_Interrupt_NotInterruptPin)
{
	g_client_task = RLM3_GetCurrentTask();
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);
	SIM_AddInterrupt([&]() { RLM3_GiveFromISR(g_client_task); });

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init = {};
	init.Pin = GPIO_PIN_12;
	init.Mode = GPIO_MODE_INPUT;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &init);

	ASSERT_ASSERTS(RLM3_Take());
	RLM3_Take();
}
