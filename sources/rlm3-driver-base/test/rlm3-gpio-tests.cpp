#include "Test.h"
#include "rlm3-gpio.h"


TEST_CASE(GPIO_Lifecycle)
{
	ASSERT(!RLM3_GPIO_IsInit());
	RLM3_GPIO_Init();
	ASSERT(RLM3_GPIO_IsInit());
	RLM3_GPIO_DeInit();
	ASSERT(!RLM3_GPIO_IsInit());
}

#define TEST_PIN_PP(PIN)                                        \
	RLM3_GPIO_WifiBootMode_SetHigh();                           \
	ASSERT(!RLM3_GPIO_WifiBootMode_IsLow());                    \
	ASSERT(RLM3_GPIO_WifiBootMode_IsHigh());                    \
	RLM3_GPIO_WifiBootMode_SetLow();                            \
	ASSERT(RLM3_GPIO_WifiBootMode_IsLow());                     \
	ASSERT(!RLM3_GPIO_WifiBootMode_IsHigh());                   \
	ASSERT(true)

TEST_CASE(GPIO_PushPull)
{
	RLM3_GPIO_Init();

	TEST_PIN_PP(CameraPowerDown);
	TEST_PIN_PP(CameraReset);
	TEST_PIN_PP(GpsReset);
	TEST_PIN_PP(InertiaChipSelect);
	TEST_PIN_PP(MotorEnable);
	TEST_PIN_PP(PowerEnable);
	TEST_PIN_PP(StatusLight);
	TEST_PIN_PP(WifiEnable);
	TEST_PIN_PP(WifiBootMode);
}

TEST_SETUP(RLM3_GPIO_Setup)
{
	if (RLM3_GPIO_IsInit())
		RLM3_GPIO_DeInit();
}
