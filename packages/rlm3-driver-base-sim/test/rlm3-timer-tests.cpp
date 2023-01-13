#include "Test.hpp"
#include "rlm3-timer.h"
#include "rlm3-task.h"
#include <functional>


static std::function<void()> g_handler_fn;

extern void RLM3_Timer2_Event_CB_ISR()
{
	if (g_handler_fn)
		g_handler_fn();
}


TEST_CASE(RLM3_Timer2_Lifecycle)
{
	ASSERT(!RLM3_Timer2_IsInit());
	RLM3_Timer2_Init(12345);
	ASSERT(RLM3_Timer2_IsInit());
	RLM3_Timer2_Deinit();
	ASSERT(!RLM3_Timer2_IsInit());
}

TEST_CASE(RLM3_Timer2_Init_AlreadyInitialized)
{
	RLM3_Timer2_Init(12345);
	ASSERT_ASSERTS(RLM3_Timer2_Init(12345));
}

TEST_CASE(RLM3_Timer2_Init_LowFrequency)
{
	ASSERT_ASSERTS(RLM3_Timer2_Init(0));
}

TEST_CASE(RLM3_Timer2_Init_HighFrequency)
{
	ASSERT_ASSERTS(RLM3_Timer2_Init(180001));
}

TEST_CASE(RLM3_Timer2_Deinit_NotInitialized)
{
	ASSERT_ASSERTS(RLM3_Timer2_Deinit());
}

TEST_CASE(RLM3_Timer2_Event_HappyCase)
{
	size_t callback_count = 0;
	RLM3_Task task = RLM3_Task_GetCurrent();
	g_handler_fn = [&] {
		callback_count++;
		RLM3_Task_GiveISR(task);
	};
	SIM_Timer2_Event();

	RLM3_Timer2_Init(1000);
	ASSERT(callback_count == 0);
	RLM3_Task_Take();
	ASSERT(callback_count == 1);
	RLM3_Timer2_Deinit();
}

TEST_SETUP(TimerTestSetup)
{
	g_handler_fn = nullptr;
}

