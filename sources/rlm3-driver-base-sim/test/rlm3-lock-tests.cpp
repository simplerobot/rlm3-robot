#include "Test.hpp"
#include "rlm3-lock.h"
#include "rlm3-sim.hpp"


TEST_CASE(RLM3_EnterCritical_HappyCase)
{
	RLM3_Lock_EnterCritical();
	RLM3_Lock_ExitCritical();
}

TEST_CASE(RLM3_EnterCritical_FailFromISR)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			RLM3_Lock_EnterCritical();
			RLM3_Lock_ExitCritical();
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_InCriticalWhenISR)
{
	auto test_fn = [] {
		RLM3_Lock_EnterCritical();
		SIM_DoInterrupt([] {});
		RLM3_Lock_ExitCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_NotExited)
{
	auto test_fn = [] {
		RLM3_Lock_EnterCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_NotEntered)
{
	auto test_fn = [] {
		RLM3_Lock_ExitCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCritical_Delay)
{
	auto test_fn = [] {
		RLM3_Lock_EnterCritical();
		RLM3_Task_Delay(1);
		RLM3_Lock_ExitCritical();
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_HappyCase)
{
	SIM_DoInterrupt([] {
		uint32_t saved = RLM3_Lock_EnterCriticalISR();
		RLM3_Lock_ExitCriticalISR(saved);
	});
}

TEST_CASE(RLM3_EnterCriticalFromISR_NotFromISR)
{
	auto test_fn = [] {
		uint32_t saved = RLM3_Lock_EnterCriticalISR();
		RLM3_Lock_ExitCriticalISR(saved);
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_NotExited)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			RLM3_Lock_EnterCriticalISR();
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_NotEntered)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			RLM3_Lock_ExitCriticalISR(1013904223);
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}

TEST_CASE(RLM3_EnterCriticalFromISR_WrongSavedValue)
{
	auto test_fn = [] {
		SIM_DoInterrupt([] {
			uint32_t saved = RLM3_Lock_EnterCriticalISR();
			RLM3_Lock_ExitCriticalISR(saved + 1);
		});
	};

	TestCaseListItem test(test_fn, "test", __FILE__, __LINE__);
	ASSERT(!test.Run());
}
