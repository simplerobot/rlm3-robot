#include "Test.hpp"
#include "rlm3-task.h"
#include "rlm3-sim.hpp"


TEST_CASE(RLM3_Delay_HappyCase)
{
	ASSERT(RLM3_GetCurrentTime() == 0);
	RLM3_Delay(10);
	ASSERT(RLM3_GetCurrentTime() == 10);
}

TEST_CASE(RLM3_DelayUntil_HappyCase)
{
	ASSERT(RLM3_GetCurrentTime() == 0);
	RLM3_DelayUntil(0, 5);
	RLM3_DelayUntil(5, 5);
	ASSERT(RLM3_GetCurrentTime() == 10);
}

TEST_CASE(RLM3_Take_HappyCase)
{
	RLM3_Give(RLM3_GetCurrentTask());

	RLM3_Take();
}

TEST_CASE(RLM3_Take_Delayed)
{
	RLM3_Task current_task = RLM3_GetCurrentTask();

	SIM_AddDelay(5);
	SIM_AddInterrupt([&]() { });
	SIM_AddDelay(15);
	SIM_AddInterrupt([&]() { RLM3_GiveFromISR(current_task); });

	RLM3_Take();
	ASSERT(RLM3_GetCurrentTime() == 20);
}

TEST_CASE(RLM3_TakeWithTimeout_HappyCase)
{
	RLM3_Give(RLM3_GetCurrentTask());

	ASSERT(RLM3_TakeWithTimeout(10));

	ASSERT(RLM3_GetCurrentTime() == 0);
}

TEST_CASE(RLM3_TakeWithTimeout_Timeout)
{
	ASSERT(!RLM3_TakeWithTimeout(10));

	ASSERT(RLM3_GetCurrentTime() == 10);
}

TEST_CASE(RLM3_TakeWithTimeout_Delayed)
{
	RLM3_Task current_task = RLM3_GetCurrentTask();

	SIM_AddDelay(5);
	SIM_AddInterrupt([&]() { });
	SIM_AddDelay(15);
	SIM_AddInterrupt([&]() { RLM3_GiveFromISR(current_task); });

	RLM3_TakeWithTimeout(30);
	ASSERT(RLM3_GetCurrentTime() == 20);
}

TEST_CASE(SIM_Give_HappyCase)
{
	SIM_Give();

	RLM3_Take();
}

TEST_CASE(SIM_Give_Delayed)
{
	SIM_AddDelay(5);
	SIM_AddInterrupt([&]() { });
	SIM_AddDelay(15);
	SIM_AddInterrupt([&]() { SIM_Give(); });

	RLM3_Take();
	ASSERT(RLM3_GetCurrentTime() == 20);
}

TEST_CASE(RLM3_EnterCritical_HappyCase)
{
	RLM3_EnterCritical();
	RLM3_ExitCritical();
}

TEST_CASE(RLM3_EnterCritical_FailFromISR)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			RLM3_EnterCritical();
			RLM3_ExitCritical();
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_InCriticalWhenISR)
{
	auto test_fn = [] {
		RLM3_EnterCritical();
		SIM_DoInterrupt([] {});
		RLM3_ExitCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_NotExited)
{
	auto test_fn = [] {
		RLM3_EnterCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_NotEntered)
{
	auto test_fn = [] {
		RLM3_ExitCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_Delay)
{
	auto test_fn = [] {
		RLM3_EnterCritical();
		RLM3_Delay(1);
		RLM3_ExitCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_HappyCase)
{
	SIM_DoInterrupt([] {
		uint32_t saved = RLM3_EnterCriticalFromISR();
		RLM3_ExitCriticalFromISR(saved);
	});
}

TEST_CASE(RLM3_EnterCriticalFromISR_NotFromISR)
{
	auto test_fn = [] {
		uint32_t saved = RLM3_EnterCriticalFromISR();
		RLM3_ExitCriticalFromISR(saved);
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_NotExited)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			RLM3_EnterCriticalFromISR();
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_NotEntered)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			RLM3_ExitCriticalFromISR(1013904223);
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_WrongSavedValue)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			uint32_t saved = RLM3_EnterCriticalFromISR();
			RLM3_ExitCriticalFromISR(saved + 1);
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}
