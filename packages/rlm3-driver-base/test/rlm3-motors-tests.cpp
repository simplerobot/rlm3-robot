#include "Test.h"
#include "rlm3-motors.h"
#include "rlm3-gpio.h"
#include "rlm3-task.h"
#include "main.h"
#include <cmath>
#include "logger.h"
#include "tim.h"


LOGGER_ZONE(TEST);


TEST_CASE(MOTORS_Lifecycle)
{
	RLM3_GPIO_Init();
	ASSERT(!RLM3_Motors_IsInit());
	ASSERT(!RLM3_Motors_IsEnabled());
	RLM3_Motors_Init();
	ASSERT(RLM3_Motors_IsInit());
	ASSERT(!RLM3_Motors_IsEnabled());
	RLM3_Motors_Enable();
	ASSERT(RLM3_Motors_IsInit());
	ASSERT(RLM3_Motors_IsEnabled());
	RLM3_Motors_Disable();
	ASSERT(RLM3_Motors_IsInit());
	ASSERT(!RLM3_Motors_IsEnabled());
	RLM3_Motors_DeInit();
	ASSERT(!RLM3_Motors_IsInit());
	ASSERT(!RLM3_Motors_IsEnabled());
	RLM3_GPIO_DeInit();
}

static bool ValidateLevels(float lf, float lb, float rf, float rb, float m, float b)
{
	size_t counts[6] = {};
	size_t total_count = 0;

	RLM3_Time start = RLM3_Time_Get();
	while (RLM3_Time_Get() - start < 100)
	{
		total_count++;
		counts[0] += HAL_GPIO_ReadPin(LEFT_FORWARD_GPIO_Port, LEFT_FORWARD_Pin);
		counts[1] += HAL_GPIO_ReadPin(LEFT_BACKWARD_GPIO_Port, LEFT_BACKWARD_Pin);
		counts[2] += HAL_GPIO_ReadPin(RIGHT_FORWARD_GPIO_Port, RIGHT_FORWARD_Pin);
		counts[3] += HAL_GPIO_ReadPin(RIGHT_BACKWARD_GPIO_Port, RIGHT_BACKWARD_Pin);
		counts[4] += HAL_GPIO_ReadPin(BLADE_MOTOR_GPIO_Port, BLADE_MOTOR_Pin);
		counts[5] += HAL_GPIO_ReadPin(BLADE_BRAKE_GPIO_Port, BLADE_BRAKE_Pin);
	}

	float expected[6] = { lf, lb, rf, rb, m, b };
	float actual[6];
	for (size_t i = 0; i < 6; i++)
		actual[i] = 1.0f * counts[i] / total_count;

	bool failed = false;
	for (size_t i = 0; i < 6; i++)
		failed = failed || (std::abs(expected[i] - actual[i]) > 0.1f);

	return !failed;
}

TEST_CASE(MOTORS_SetWheels_HappyCase)
{
	RLM3_GPIO_Init();
	RLM3_Motors_Init();
	RLM3_Motors_Enable();

	ASSERT(ValidateLevels(0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

	RLM3_Motors_SetWheels(127, -128);
	ASSERT(ValidateLevels(1.0, 0.0, 0.0, 1.0, 0.0, 0.0));

	RLM3_Motors_SetWheels(64, -33);
	ASSERT(ValidateLevels(0.5, 0.0, 0.0, 0.25, 0.0, 0.0));

	RLM3_Motors_SetWheels(-33, 64);
	ASSERT(ValidateLevels(0.0, 0.25, 0.5, 0.0, 0.0, 0.0));

	RLM3_Motors_SetWheels(-128, 127);
	ASSERT(ValidateLevels(0.0, 1.0, 1.0, 0.0, 0.0, 0.0));
}

TEST_CASE(MOTORS_SetWheels_Reenable)
{
	RLM3_GPIO_Init();
	RLM3_Motors_Init();
	RLM3_Motors_Enable();
	RLM3_Motors_SetWheels(127, -128);
	RLM3_Motors_Disable();
	RLM3_Motors_Enable();

	ASSERT(ValidateLevels(0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
}


TEST_TEARDOWN(MOTORS_Teardown)
{
	if (RLM3_Motors_IsInit())
	{
		if (RLM3_Motors_IsEnabled())
			RLM3_Motors_Disable();
		RLM3_Motors_DeInit();
	}
}
