#include "Test.hpp"
#include "rlm3-gpio.h"


TEST_CASE(GPIO_Lifecycle)
{
	ASSERT(!RLM3_GPIO_IsInit());
	RLM3_GPIO_Init();
	ASSERT(RLM3_GPIO_IsInit());
	RLM3_GPIO_DeInit();
	ASSERT(!RLM3_GPIO_IsInit());
}

TEST_CASE(GPIO_GetSet)
{
	RLM3_GPIO_Init();

	ASSERT(!RLM3_GPIO_CameraPowerDown_IsHigh());
	ASSERT(RLM3_GPIO_CameraPowerDown_IsLow());
	RLM3_GPIO_CameraPowerDown_SetHigh();
	ASSERT(RLM3_GPIO_CameraPowerDown_IsHigh());
	ASSERT(!RLM3_GPIO_CameraPowerDown_IsLow());
	RLM3_GPIO_CameraPowerDown_SetLow();
	ASSERT(!RLM3_GPIO_CameraPowerDown_IsHigh());
	ASSERT(RLM3_GPIO_CameraPowerDown_IsLow());

	ASSERT(!RLM3_GPIO_CameraReset_IsHigh());
	ASSERT(RLM3_GPIO_CameraReset_IsLow());
	RLM3_GPIO_CameraReset_SetHigh();
	ASSERT(RLM3_GPIO_CameraReset_IsHigh());
	ASSERT(!RLM3_GPIO_CameraReset_IsLow());
	RLM3_GPIO_CameraReset_SetLow();
	ASSERT(!RLM3_GPIO_CameraReset_IsHigh());
	ASSERT(RLM3_GPIO_CameraReset_IsLow());

	ASSERT(!RLM3_GPIO_GpsReset_IsHigh());
	ASSERT(RLM3_GPIO_GpsReset_IsLow());
	RLM3_GPIO_GpsReset_SetHigh();
	ASSERT(RLM3_GPIO_GpsReset_IsHigh());
	ASSERT(!RLM3_GPIO_GpsReset_IsLow());
	RLM3_GPIO_GpsReset_SetLow();
	ASSERT(!RLM3_GPIO_GpsReset_IsHigh());
	ASSERT(RLM3_GPIO_GpsReset_IsLow());

	ASSERT(!RLM3_GPIO_InertiaChipSelect_IsHigh());
	ASSERT(RLM3_GPIO_InertiaChipSelect_IsLow());
	RLM3_GPIO_InertiaChipSelect_SetHigh();
	ASSERT(RLM3_GPIO_InertiaChipSelect_IsHigh());
	ASSERT(!RLM3_GPIO_InertiaChipSelect_IsLow());
	RLM3_GPIO_InertiaChipSelect_SetLow();
	ASSERT(!RLM3_GPIO_InertiaChipSelect_IsHigh());
	ASSERT(RLM3_GPIO_InertiaChipSelect_IsLow());

	ASSERT(!RLM3_GPIO_StatusLight_IsHigh());
	ASSERT(RLM3_GPIO_StatusLight_IsLow());
	RLM3_GPIO_StatusLight_SetHigh();
	ASSERT(RLM3_GPIO_StatusLight_IsHigh());
	ASSERT(!RLM3_GPIO_StatusLight_IsLow());
	RLM3_GPIO_StatusLight_SetLow();
	ASSERT(!RLM3_GPIO_StatusLight_IsHigh());
	ASSERT(RLM3_GPIO_StatusLight_IsLow());

	ASSERT(RLM3_GPIO_PowerEnable_IsHigh());
	ASSERT(!RLM3_GPIO_PowerEnable_IsLow());
	RLM3_GPIO_PowerEnable_SetLow();
	ASSERT(!RLM3_GPIO_PowerEnable_IsHigh());
	ASSERT(RLM3_GPIO_PowerEnable_IsLow());
	RLM3_GPIO_PowerEnable_SetHigh();
	ASSERT(RLM3_GPIO_PowerEnable_IsHigh());
	ASSERT(!RLM3_GPIO_PowerEnable_IsLow());

	ASSERT(RLM3_GPIO_MotorEnable_IsHigh());
	ASSERT(!RLM3_GPIO_MotorEnable_IsLow());
	RLM3_GPIO_MotorEnable_SetLow();
	ASSERT(!RLM3_GPIO_MotorEnable_IsHigh());
	ASSERT(RLM3_GPIO_MotorEnable_IsLow());
	RLM3_GPIO_MotorEnable_SetHigh();
	ASSERT(RLM3_GPIO_MotorEnable_IsHigh());
	ASSERT(!RLM3_GPIO_MotorEnable_IsLow());

	ASSERT(!RLM3_GPIO_WifiEnable_IsHigh());
	ASSERT(RLM3_GPIO_WifiEnable_IsLow());
	RLM3_GPIO_WifiEnable_SetHigh();
	ASSERT(RLM3_GPIO_WifiEnable_IsHigh());
	ASSERT(!RLM3_GPIO_WifiEnable_IsLow());
	RLM3_GPIO_WifiEnable_SetLow();
	ASSERT(!RLM3_GPIO_WifiEnable_IsHigh());
	ASSERT(RLM3_GPIO_WifiEnable_IsLow());

	ASSERT(!RLM3_GPIO_WifiBootMode_IsHigh());
	ASSERT(RLM3_GPIO_WifiBootMode_IsLow());
	RLM3_GPIO_WifiBootMode_SetHigh();
	ASSERT(RLM3_GPIO_WifiBootMode_IsHigh());
	ASSERT(!RLM3_GPIO_WifiBootMode_IsLow());
	RLM3_GPIO_WifiBootMode_SetLow();
	ASSERT(!RLM3_GPIO_WifiBootMode_IsHigh());
	ASSERT(RLM3_GPIO_WifiBootMode_IsLow());

	ASSERT(!RLM3_GPIO_WifiReset_IsHigh());
	ASSERT(RLM3_GPIO_WifiReset_IsLow());
	RLM3_GPIO_WifiReset_SetHigh();
	ASSERT(RLM3_GPIO_WifiReset_IsHigh());
	ASSERT(!RLM3_GPIO_WifiReset_IsLow());
	RLM3_GPIO_WifiReset_SetLow();
	ASSERT(!RLM3_GPIO_WifiReset_IsHigh());
	ASSERT(RLM3_GPIO_WifiReset_IsLow());
}


/*

extern bool RLM3_GPIO_BaseSense_IsHigh();
extern bool RLM3_GPIO_BaseSense_IsLow();
extern bool RLM3_GPIO_GpsPulse_IsHigh();
extern bool RLM3_GPIO_GpsPulse_IsLow();
extern bool RLM3_GPIO_StopA_IsHigh();
extern bool RLM3_GPIO_StopA_IsLow();
extern bool RLM3_GPIO_StopB_IsHigh();
extern bool RLM3_GPIO_StopB_IsLow();

extern void RLM3_GPIO_BaseSense_Event_CB_ISR();
extern void RLM3_GPIO_GpsPulse_Event_CB_ISR();
extern void RLM3_GPIO_StopA_Event_CB_ISR();
extern void RLM3_GPIO_StopB_Event_CB_ISR();

extern void SIM_GPIO_BaseSense_SetHigh();
extern void SIM_GPIO_BaseSense_SetLow();
extern void SIM_GPIO_GpsPulse_SetHigh();
extern void SIM_GPIO_GpsPulse_SetLow();
extern void SIM_GPIO_StopA_SetHigh();
extern void SIM_GPIO_StopA_SetLow();
extern void SIM_GPIO_StopB_SetHigh();
extern void SIM_GPIO_StopB_SetLow();

extern void SIM_GPIO_BaseSense_Event();
extern void SIM_GPIO_GpsPulse_Event();
extern void SIM_GPIO_StopA_Event();
extern void SIM_GPIO_StopB_Event();
*/


/*
#include "Test.hpp"
#include "rlm3-gpio.h"
#include "rlm3-task.h"
#include "rlm3-sim.hpp"


size_t g_exti_count = 0;
RLM3_Task g_client_task = NULL;

extern void RLM3_EXTI12_Callback()
{
	g_exti_count++;
	RLM3_Task_GiveISR(g_client_task);
}

TEST_CASE(SIM_GPIO_Interrupt_HappyCase)
{
	g_client_task = RLM3_Task_GetCurrent();
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
	RLM3_Task_Take();
	ASSERT(g_exti_count == 1);
	ASSERT(RLM3_Time_Get() == 10);
}

TEST_CASE(SIM_GPIO_Interrupt_ClockNotEnabled)
{
	g_client_task = RLM3_Task_GetCurrent();
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);
	SIM_AddInterrupt([&]() { RLM3_Task_GiveISR(g_client_task); });

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init = {};
	init.Pin = GPIO_PIN_12;
	init.Mode = GPIO_MODE_IT_RISING;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &init);
	__HAL_RCC_GPIOA_CLK_DISABLE();

	ASSERT_ASSERTS(RLM3_Task_Take());
	RLM3_Task_Take();
}

TEST_CASE(SIM_GPIO_Interrupt_PinNotEnabled)
{
	g_client_task = RLM3_Task_GetCurrent();
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);
	SIM_AddInterrupt([&]() { RLM3_Task_GiveISR(g_client_task); });

	__HAL_RCC_GPIOA_CLK_ENABLE();

	ASSERT_ASSERTS(RLM3_Task_Take());
	RLM3_Task_Take();
}

TEST_CASE(SIM_GPIO_Interrupt_NotInterruptPin)
{
	g_client_task = RLM3_Task_GetCurrent();
	SIM_GPIO_Interrupt(GPIOA, GPIO_PIN_12);
	SIM_AddInterrupt([&]() { RLM3_Task_GiveISR(g_client_task); });

	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init = {};
	init.Pin = GPIO_PIN_12;
	init.Mode = GPIO_MODE_INPUT;
	init.Pull = GPIO_NOPULL;
	init.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &init);

	ASSERT_ASSERTS(RLM3_Task_Take());
	RLM3_Task_Take();
}

 */
