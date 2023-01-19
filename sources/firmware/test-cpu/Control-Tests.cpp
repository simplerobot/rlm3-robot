#include "Test.hpp"
#include "Control.hpp"
#include "rlm3-sim.hpp"
#include "rlm3-motors.h"
#include "CommInput.hpp"
#include "logger.h"


LOGGER_ZONE(TEST);


TEST_CASE(Control_Abort)
{
	SIM_AddInterrupt([] { Control_AbortTaskISR(); });

	Control_RunTask();
}

TEST_CASE(Control_AbortRestart)
{
	SIM_AddInterrupt([] { Control_AbortTaskISR(); });
	SIM_AddInterrupt([] { Control_AbortTaskISR(); });

	Control_RunTask();
	Control_RunTask();
}

static void ValidateMotor(const char* name, float expected, float actual)
{
	if (expected == 0.0f && actual != 0.0f)
		FAIL("Motor %s expected %f actual %f diff %f", name, expected, actual, expected - actual);
	if (std::abs(expected - actual) > 0.01f)
		FAIL("Motor %s expected %f actual %f diff %f", name, expected, actual, expected - actual);
}

static void ValidateMotors(float expected_left, float expected_right, float expected_blade)
{
	ValidateMotor("left", expected_left, SIM_Motors_GetLeft());
	ValidateMotor("right", expected_right, SIM_Motors_GetRight());
	ValidateMotor("blade", expected_blade, SIM_Motors_GetBlade());
}

TEST_CASE(Control_HappyCase)
{
	SIM_AddTask([] { ValidateMotors(0.0, 0.0, 0.0); });
	SIM_AddDelay(200);
	SIM_AddTask([] { CommInput_Control_Callback(RLM3_Time_Get(), 0.5, 0.3, 0.8); });
	SIM_AddTask([] { ValidateMotors(0.5, 0.3, 0.8); });
	SIM_AddDelay(60);
	SIM_AddTask([] { ValidateMotors(0.5, 0.3, 0.8); });
	SIM_AddTask([] { CommInput_Control_Callback(RLM3_Time_Get(), -0.2, -0.3, -0.5); });
	SIM_AddTask([] { ValidateMotors(-0.2, -0.3, -0.5); });
	SIM_AddInterrupt([] { Control_AbortTaskISR(); });

	Control_RunTask();
}

TEST_CASE(Control_LostSignal)
{
	SIM_AddTask([] { ValidateMotors(0.0, 0.0, 0.0); });
	SIM_AddDelay(200);
	SIM_AddTask([] { CommInput_Control_Callback(RLM3_Time_Get(), 0.5, 0.3, 0.8); });
	SIM_AddTask([] { ValidateMotors(0.5, 0.3, 0.8); });
	SIM_AddDelay(100);
	SIM_AddTask([] { ValidateMotors(0.5, 0.3, 0.8); });
	SIM_AddDelay(1);
	SIM_AddTask([] { ValidateMotors(0.0, 0.0, 0.0); });
	SIM_AddInterrupt([] { Control_AbortTaskISR(); });

	Control_RunTask();
}

TEST_CASE(Control_LateSignal)
{
	SIM_AddDelay(200);
	SIM_AddTask([] { CommInput_Control_Callback(RLM3_Time_Get() - 100, 0.5, 0.3, 0.8); });
	SIM_AddTask([] { ValidateMotors(0.0, 0.0, 0.0); });
	SIM_AddInterrupt([] { Control_AbortTaskISR(); });

	Control_RunTask();
}

