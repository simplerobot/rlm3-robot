#include "Test.h"
#include "rlm3-timer.h"
#include "rlm3-task.h"
#include "logger.h"


#include "main.h"
LOGGER_ZONE(TEST);


typedef void (*TimerFn)();
static TimerFn g_timer_fn = nullptr;


extern void RLM3_Timer2_Event_CB_ISR()
{
	if (g_timer_fn != nullptr)
		g_timer_fn();
}


TEST_CASE(RLM3_Timer2_Lifecycle)
{
	ASSERT(!RLM3_Timer2_IsInit());
	RLM3_Timer2_Init(1000);
	ASSERT(RLM3_Timer2_IsInit());
	RLM3_Timer2_Deinit();
	ASSERT(!RLM3_Timer2_IsInit());
}


TEST_CASE(RLM3_Timer2_HappyCase)
{
	static size_t g_count = 0;
	g_timer_fn = [] { g_count++; };

	RLM3_Timer2_Init(5000);
	RLM3_Task_Delay(100);
	RLM3_Timer2_Deinit();

	ASSERT(g_count >= 498 && g_count <= 502);
}

TEST_TEARDOWN(TIMER_CLEANUP)
{
	g_timer_fn = nullptr;
	if (RLM3_Timer2_IsInit())
		RLM3_Timer2_Deinit();
}

