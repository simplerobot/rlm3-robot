#include "rlm3-gpio.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"


static bool g_is_init = false;

static void InitPins();


extern void RLM3_GPIO_Init()
{
	ASSERT(!SIM_IsISR());
	ASSERT(!g_is_init);
	g_is_init = true;
	InitPins();
}

extern void RLM3_GPIO_DeInit()
{
	ASSERT(!SIM_IsISR());
	ASSERT(g_is_init);
	g_is_init = false;
}

extern bool RLM3_GPIO_IsInit()
{
	ASSERT(!SIM_IsISR());
	return g_is_init;
}

#define DEFINE_GPIO_VARIABLE(NAME)                   \
	static bool g_ ## NAME ## _is_high = false;

#define DEFINE_GPIO_SETHIGH(NAME)                    \
	extern void RLM3_GPIO_ ## NAME ## _SetHigh()     \
	{                                                \
		ASSERT(RLM3_GPIO_IsInit());                  \
		g_ ## NAME ## _is_high = true;               \
	}

#define DEFINE_GPIO_SETLOW(NAME)                     \
	extern void RLM3_GPIO_ ## NAME ## _SetLow()      \
	{                                                \
		ASSERT(RLM3_GPIO_IsInit());                  \
		g_ ## NAME ## _is_high = false;              \
	}

#define DEFINE_GPIO_ISHIGH(NAME)                     \
	extern bool RLM3_GPIO_ ## NAME ## _IsHigh()      \
	{                                                \
		ASSERT(RLM3_GPIO_IsInit());                  \
		return g_ ## NAME ## _is_high;               \
	}

#define DEFINE_GPIO_ISLOW(NAME)                      \
	extern bool RLM3_GPIO_ ## NAME ## _IsLow()       \
	{                                                \
		ASSERT(RLM3_GPIO_IsInit());                  \
		return !g_ ## NAME ## _is_high;              \
	}

// NOTE: SetHigh and SetLow do not automatically call the event handler.  This allows more flexibility while testing.

#define DEFINE_SIM_SETHIGH(NAME)                     \
	extern void SIM_GPIO_ ## NAME ## _SetHigh()      \
	{                                                \
		SIM_AddInterrupt([]() {                      \
			ASSERT(g_is_init);                       \
			g_ ## NAME ## _is_high = true;           \
		});                                          \
	}

#define DEFINE_SIM_SETLOW(NAME)                      \
	extern void SIM_GPIO_ ## NAME ## _SetLow()       \
	{                                                \
		SIM_AddInterrupt([]() {                      \
			ASSERT(g_is_init);                       \
			g_ ## NAME ## _is_high = false;          \
		});                                          \
	}

#define DEFINE_SIM_EVENT(NAME)                       \
	extern void SIM_GPIO_ ## NAME ## _Event()        \
	{                                                \
		SIM_AddInterrupt([]() {                      \
			ASSERT(g_is_init);                       \
			RLM3_GPIO_ ## NAME ## _Event_CB_ISR();   \
		});                                          \
	}

#define DEFINE_GPIO_CALLBACK(NAME)                   \
	extern __attribute__((weak)) void RLM3_GPIO_ ## NAME ## _Event_CB_ISR() \
	{                                                \
		ASSERT(false);                               \
	}

#define DEFINE_GPIO_PP(NAME)                         \
		DEFINE_GPIO_VARIABLE(NAME)                   \
		DEFINE_GPIO_SETHIGH(NAME)                    \
		DEFINE_GPIO_SETLOW(NAME)                     \
		DEFINE_GPIO_ISHIGH(NAME)                     \
		DEFINE_GPIO_ISLOW(NAME)

#define DEFINE_GPIO_INT(NAME)                        \
		DEFINE_GPIO_VARIABLE(NAME)                   \
		DEFINE_GPIO_ISHIGH(NAME)                     \
		DEFINE_GPIO_ISLOW(NAME)                      \
		DEFINE_SIM_SETHIGH(NAME)                     \
		DEFINE_SIM_SETLOW(NAME)                      \
		DEFINE_SIM_EVENT(NAME)                       \
		DEFINE_GPIO_CALLBACK(NAME)


DEFINE_GPIO_PP(CameraPowerDown)
DEFINE_GPIO_PP(CameraReset)
DEFINE_GPIO_PP(GpsReset)
DEFINE_GPIO_PP(InertiaChipSelect)
DEFINE_GPIO_PP(MotorEnable)
DEFINE_GPIO_PP(PowerEnable)
DEFINE_GPIO_PP(StatusLight)
DEFINE_GPIO_PP(WifiEnable)
DEFINE_GPIO_PP(WifiBootMode)
DEFINE_GPIO_PP(WifiReset)
DEFINE_GPIO_INT(BaseSense)
DEFINE_GPIO_INT(GpsPulse)
DEFINE_GPIO_INT(StopA)
DEFINE_GPIO_INT(StopB)


static void InitPins()
{
	g_CameraPowerDown_is_high = false;
	g_CameraReset_is_high = false;
	g_CameraReset_is_high = false;
	g_GpsReset_is_high = false;
	g_InertiaChipSelect_is_high = false;
	g_StatusLight_is_high = false;
	g_PowerEnable_is_high = true;
	g_MotorEnable_is_high = true;
	g_WifiEnable_is_high = false;
	g_WifiBootMode_is_high = false;
	g_WifiReset_is_high = false;
}

TEST_SETUP(RLM3_GPIO_Setup)
{
	g_is_init = false;
	InitPins();
}

