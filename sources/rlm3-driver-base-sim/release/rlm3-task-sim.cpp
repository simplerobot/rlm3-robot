#include "rlm3-task.h"
#include "rlm3-sim.hpp"
#include "rlm3-lock.h"
#include "Test.hpp"


static RLM3_Time g_current_time = 0;
static bool g_is_task_active = false;

static const RLM3_Task k_task_id = (RLM3_Task)0xDECADE; // We only have one task, just use a known token


extern RLM3_Time RLM3_Time_Get()
{
	if (SIM_IsISR())
		FAIL("RLM3_GetCurrentTime called from an ISR routine.");
	return g_current_time;
}

extern RLM3_Time RLM3_Time_GetISR()
{
	if (!SIM_IsISR())
		FAIL("RLM3_GetCurrentTimeFromISR called from a non-ISR routine.");
	return g_current_time;
}

extern void RLM3_Task_Yield()
{
	if (SIM_IsISR())
		FAIL("RLM3_Yield called from an ISR routine.");
	if (!SIM_IsMainTask())
		FAIL("RLM3_Yield called from a secondary task.");
	if (SIM_IsInCritical())
		FAIL("RLM3_Yield called while in a critical section.");
	// Nothing to do since we don't support threads in the simulator.
}

extern void RLM3_Task_Delay(RLM3_Time delay_ms)
{
	if (SIM_IsISR())
		FAIL("RLM3_Delay called from an ISR routine.");
	if (!SIM_IsMainTask())
		FAIL("RLM3_Delay called from a secondary task.");
	if (SIM_IsInCritical())
		FAIL("RLM3_Delay called while in a critical section.");
	g_current_time += delay_ms;
}

extern void RLM3_Task_DelayUntil(RLM3_Time start_time, RLM3_Time delay_ms)
{
	if (SIM_IsISR())
		FAIL("RLM3_Delay called from an ISR routine.");
	if (!SIM_IsMainTask())
		FAIL("RLM3_Delay called from a secondary task.");
	if (g_current_time < start_time)
		FAIL("RLM3_DelayUntil called with a futuristic start time. %d vs %d", (int)g_current_time, (int)start_time);
	if (SIM_IsInCritical())
		FAIL("RLM3_DelayUntil called while in a critical section.");
	if (g_current_time - start_time < delay_ms)
		RLM3_Task_Delay(delay_ms + start_time - g_current_time);
}

extern RLM3_Task RLM3_Task_Create(RLM3_Task_Fn fn, size_t stack_size_words, const char* name)
{
	FAIL("The simulator does not support multiple tasks.");
	return nullptr;
}

extern RLM3_Task RLM3_Task_GetCurrent()
{
	if (!SIM_IsMainTask())
		FAIL("RLM3_Task_GetCurrent called from a secondary task.");
	return k_task_id;
}

extern void RLM3_Task_Give(RLM3_Task task)
{
	if (SIM_IsISR())
		FAIL("RLM3_Give called from an ISR routine.");
	if (task == nullptr)
		return;
	if (task != k_task_id)
		FAIL("RLM3_Give called with an invalid task pointer %p", task);
	g_is_task_active = true;
}

extern void RLM3_Task_GiveISR(RLM3_Task task)
{
	if (!SIM_IsISR())
		FAIL("RLM3_GiveFromISR called from a non-ISR routine.");
	if (task == nullptr)
		return;
	if (task != k_task_id)
		FAIL("RLM3_GiveFromISR called with an invalid task pointer %p", task);
	g_is_task_active = true;
}

extern void SIM_Give()
{
	g_is_task_active = true;
}

extern void RLM3_Task_Take()
{
	if (SIM_IsISR())
		FAIL("RLM3_Take called from an ISR routine.");
	if (!SIM_IsMainTask())
		FAIL("RLM3_Take called from a secondary task.");
	if (SIM_IsInCritical())
		FAIL("RLM3_Take called while in a critical section.");
	while (!g_is_task_active)
	{
		if (!SIM_HasNextHandler())
			FAIL("Test thread sleeping with no more interrupts available to wake it up. (Make sure you are calling RLM3_Give or RLM3_GiveFromISR or SIM_Give)");
		g_current_time = SIM_GetNextHandlerTime();
		SIM_RunNextHandler();
	}
	g_is_task_active = false;
}

extern bool RLM3_Task_TakeWithTimeout(RLM3_Time timeout_ms)
{
	if (SIM_IsISR())
		FAIL("RLM3_TakeWithTimeout called from an ISR routine.");
	if (!SIM_IsMainTask())
		FAIL("RLM3_TakeWithTimeout called from a secondary task.");
	if (SIM_IsInCritical())
		FAIL("RLM3_TakeWithTimeout called while in a critical section.");
	RLM3_Time end_time = g_current_time + timeout_ms;
	while (!g_is_task_active)
	{
		if (!SIM_HasNextHandler())
		{
			g_current_time = end_time;
			return false;
		}
		RLM3_Time next_time = SIM_GetNextHandlerTime();
		if (next_time > end_time)
		{
			g_current_time = end_time;
			return false;
		}
		g_current_time = next_time;
		SIM_RunNextHandler();
	}
	g_is_task_active = false;
	return true;
}

extern bool RLM3_Task_TakeUntil(RLM3_Time start_time, RLM3_Time delay_ms)
{
	if (SIM_IsISR())
		FAIL("RLM3_TakeWithTimeout called from an ISR routine.");
	if (!SIM_IsMainTask())
		FAIL("RLM3_TakeWithTimeout called from a secondary task.");
	if (SIM_IsInCritical())
		FAIL("RLM3_TakeWithTimeout called while in a critical section.");
	ASSERT(g_current_time >= start_time);
	if (SIM_IsInCritical())
		FAIL("RLM3_TakeUntil called while in a critical section.");
	if (g_current_time - start_time >= delay_ms)
		return false;
	return RLM3_Task_TakeWithTimeout(delay_ms + start_time - g_current_time);
}


TEST_TEARDOWN(TASK_TEARDOWN)
{
	g_current_time = 0;
	g_is_task_active = false;
}

