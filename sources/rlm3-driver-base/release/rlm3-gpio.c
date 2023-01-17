#include "rlm3-gpio.h"
#include "main.h"
#include "gpio.h"
#include "Assert.h"


static bool g_is_init = false;


extern void RLM3_GPIO_Init()
{
	ASSERT(!g_is_init);
	if (!g_is_init)
	{
		MX_GPIO_Init();
		g_is_init = true;
	}
}

extern void RLM3_GPIO_DeInit()
{
	ASSERT(g_is_init);
	if (g_is_init)
	{
		HAL_GPIO_DeInit(CAMERA_POWER_DOWN_GPIO_Port, CAMERA_POWER_DOWN_Pin);
		HAL_GPIO_DeInit(GPIOC, STOP_B_Pin|BASE_SENSE_Pin|STOP_A_Pin);
		HAL_GPIO_DeInit(GPIOB, TEST_TX_Pin|TEST_RX_Pin);
		HAL_GPIO_DeInit(GPS_RESET_GPIO_Port, GPS_RESET_Pin);
		HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15);
		HAL_GPIO_DeInit(GPIOD, CAMERA_RESET_Pin|INERTIA_CHIP_SELECT_Pin|STATUS_LIGHT_Pin);
		HAL_GPIO_DeInit(GPIOG, POWER_ENABLE_Pin|MOTOR_ENABLE_Pin|WIFI_ENABLE_Pin|WIFI_BOOT_MODE_Pin|WIFI_RESET_Pin);
		HAL_GPIO_DeInit(CAMERA_CLOCK_GPIO_Port, CAMERA_CLOCK_Pin);
		HAL_GPIO_DeInit(GPS_PULSE_GPIO_Port, GPS_PULSE_Pin);

		g_is_init = false;
	}
}

extern bool RLM3_GPIO_IsInit()
{
	return g_is_init;
}

// Using the preprocessor to define all these methods to avoid copy paste errors.

#define DEFINE_GPIO_SETHIGH(ID, NAME)                                               \
	extern void RLM3_GPIO_ ## NAME ## _SetHigh()                                    \
	{                                                                               \
		ASSERT(RLM3_GPIO_IsInit());                                                 \
		HAL_GPIO_WritePin(ID ## _GPIO_Port, ID ## _Pin, GPIO_PIN_SET);              \
	}

#define DEFINE_GPIO_SETLOW(ID, NAME)                                                \
	extern void RLM3_GPIO_ ## NAME ## _SetLow()                                     \
	{                                                                               \
		ASSERT(RLM3_GPIO_IsInit());                                                 \
		HAL_GPIO_WritePin(ID ## _GPIO_Port, ID ## _Pin, GPIO_PIN_RESET);            \
	}

#define DEFINE_GPIO_ISHIGH(ID, NAME)                                                \
	extern bool RLM3_GPIO_ ## NAME ## _IsHigh()                                     \
	{                                                                               \
		ASSERT(RLM3_GPIO_IsInit());                                                 \
		return (HAL_GPIO_ReadPin(ID ## _GPIO_Port, ID ## _Pin) != GPIO_PIN_RESET);  \
	}

#define DEFINE_GPIO_ISLOW(ID, NAME)                                                 \
	extern bool RLM3_GPIO_ ## NAME ## _IsLow()                                      \
	{                                                                               \
		ASSERT(RLM3_GPIO_IsInit());                                                 \
		return (HAL_GPIO_ReadPin(ID ## _GPIO_Port, ID ## _Pin) == GPIO_PIN_RESET);  \
	}

#define DEFINE_GPIO_CALLBACK(ID, NAME)                                              \
	extern __attribute__((weak)) void RLM3_GPIO_ ## NAME ## _Event_CB_ISR()           \
	{                                                                               \
		ASSERT(false); \
	}


#define DEFINE_GPIO_SET(ID, NAME) DEFINE_GPIO_SETHIGH(ID, NAME) DEFINE_GPIO_SETLOW(ID, NAME)
#define DEFINE_GPIO_GET(ID, NAME) DEFINE_GPIO_ISHIGH(ID, NAME) DEFINE_GPIO_ISLOW(ID, NAME)

#define DEFINE_GPIO_PP(ID, NAME) DEFINE_GPIO_SET(ID, NAME) DEFINE_GPIO_GET(ID, NAME)
#define DEFINE_GPIO_INT(ID, NAME) DEFINE_GPIO_GET(ID, NAME) DEFINE_GPIO_CALLBACK(ID, NAME)

DEFINE_GPIO_PP(CAMERA_POWER_DOWN, CameraPowerDown)
DEFINE_GPIO_PP(CAMERA_RESET, CameraReset)
DEFINE_GPIO_PP(GPS_RESET, GpsReset)
DEFINE_GPIO_PP(INERTIA_CHIP_SELECT, InertiaChipSelect)
DEFINE_GPIO_PP(MOTOR_ENABLE, MotorEnable)
DEFINE_GPIO_PP(POWER_ENABLE, PowerEnable)
DEFINE_GPIO_PP(STATUS_LIGHT, StatusLight)
DEFINE_GPIO_PP(WIFI_ENABLE, WifiEnable)
DEFINE_GPIO_PP(WIFI_BOOT_MODE, WifiBootMode)
DEFINE_GPIO_PP(WIFI_RESET, WifiReset)
DEFINE_GPIO_INT(BASE_SENSE, BaseSense)
DEFINE_GPIO_INT(GPS_PULSE, GpsPulse)
DEFINE_GPIO_INT(STOP_A, StopA)
DEFINE_GPIO_INT(STOP_B, StopB)

extern void EXTI2_IRQHandler()
{
	RLM3_GPIO_BaseSense_Event_CB_ISR();
}

extern void EXTI15_10_IRQHandler()
{
	const uint32_t supported_interrupts = EXTI_PR_PR11 | EXTI_PR_PR12 | EXTI_PR_PR13;

	// Get all pending interrupts.
	uint32_t pending_interrupts = EXTI->PR;

	// Clear any pending interrupts that will be handled in this function.
	EXTI->PR = pending_interrupts & supported_interrupts;

	// Call the handlers for each handled interrupt.
	if ((pending_interrupts & EXTI_PR_PR11) != 0)
		RLM3_GPIO_StopA_Event_CB_ISR();
	if ((pending_interrupts & EXTI_PR_PR12) != 0)
		RLM3_GPIO_GpsPulse_Event_CB_ISR();
	if ((pending_interrupts & EXTI_PR_PR13) != 0)
		RLM3_GPIO_StopB_Event_CB_ISR();
}
