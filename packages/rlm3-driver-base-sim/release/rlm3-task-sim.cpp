#include "rlm3-task.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"


static RLM3_Time g_current_time = 0;
static bool g_is_task_active = false;

static const RLM3_Task k_task_id = (RLM3_Task)0xDECADE; // We only have one task, just use a known token

static size_t g_critical_count = 0;
static size_t g_critical_isr_count = 0;



extern RLM3_Time RLM3_GetCurrentTime()
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_GetCurrentTime called from an ISR routine.");
	return g_current_time;
}

extern RLM3_Time RLM3_GetCurrentTimeFromISR()
{
	if (!SIM_RLM3_Is_IRQ())
		FAIL("RLM3_GetCurrentTimeFromISR called from a non-ISR routine.");
	return g_current_time;
}

extern void RLM3_Yield()
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_Yield called from an ISR routine.");
	if (g_critical_count != 0 || g_critical_isr_count != 0)
		FAIL("RLM3_Yield called while in a critical section.");
	// Nothing to do since we don't support threads in the simulator.
}

extern void RLM3_Delay(RLM3_Time delay_ms)
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_Delay called from an ISR routine.");
	if (g_critical_count != 0 || g_critical_isr_count != 0)
		FAIL("RLM3_Delay called while in a critical section.");
	g_current_time += delay_ms;
}

extern void RLM3_DelayUntil(RLM3_Time start_time, RLM3_Time delay_ms)
{
	if (g_current_time < start_time)
		FAIL("RLM3_DelayUntil called with a futuristic start time. %d vs %d", (int)g_current_time, (int)start_time);
	if (g_critical_count != 0 || g_critical_isr_count != 0)
		FAIL("RLM3_DelayUntil called while in a critical section.");
	if (g_current_time - start_time < delay_ms)
		RLM3_Delay(delay_ms + start_time - g_current_time);
}

extern RLM3_Task RLM3_GetCurrentTask()
{
	return k_task_id;
}

extern void RLM3_Give(RLM3_Task task)
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_Give called from an ISR routine.");
	if (task == nullptr)
		return;
	if (task != k_task_id)
		FAIL("RLM3_Give called with an invalid task pointer %p", task);
	g_is_task_active = true;
}

extern void RLM3_GiveFromISR(RLM3_Task task)
{
	if (!SIM_RLM3_Is_IRQ())
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

extern void RLM3_Take()
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_Take called from an ISR routine.");
	if (g_critical_count != 0 || g_critical_isr_count != 0)
		FAIL("RLM3_Take called while in a critical section.");
	while (!g_is_task_active)
	{
		if (!SIM_HasNextInterrupt())
			FAIL("Test thread sleeping with no more interrupts available to wake it up. (Make sure you are calling RLM3_Give or RLM3_GiveFromISR or SIM_Give)");
		g_current_time = SIM_GetNextInterruptTime();
		SIM_RunNextInterrupt();
	}
	g_is_task_active = false;
}

extern bool RLM3_TakeWithTimeout(RLM3_Time timeout_ms)
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_TakeWithTimeout called from an ISR routine.");
	if (g_critical_count != 0 || g_critical_isr_count != 0)
		FAIL("RLM3_TakeWithTimeout called while in a critical section.");
	RLM3_Time end_time = g_current_time + timeout_ms;
	while (!g_is_task_active)
	{
		if (!SIM_HasNextInterrupt())
		{
			g_current_time = end_time;
			return false;
		}
		RLM3_Time next_time = SIM_GetNextInterruptTime();
		if (next_time > end_time)
		{
			g_current_time = end_time;
			return false;
		}
		g_current_time = next_time;
		SIM_RunNextInterrupt();
	}
	g_is_task_active = false;
	return true;
}

extern bool RLM3_TakeUntil(RLM3_Time start_time, RLM3_Time delay_ms)
{
	ASSERT(g_current_time >= start_time);
	if (g_critical_count != 0 || g_critical_isr_count != 0)
		FAIL("RLM3_TakeUntil called while in a critical section.");
	if (g_current_time - start_time >= delay_ms)
		return false;
	return RLM3_TakeWithTimeout(delay_ms + start_time - g_current_time);
}

extern void RLM3_EnterCritical()
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_EnterCritical called from an ISR routine.");
	g_critical_count++;
}

static uint32_t ToIsrCriticalRegionSavedLevel(uint32_t x)
{
	return 1664525 * x + 1013904223;
}

extern uint32_t RLM3_EnterCriticalFromISR()
{
	if (!SIM_RLM3_Is_IRQ())
		FAIL("RLM3_EnterCriticalFromISR called from a non-ISR routine.");
	g_critical_isr_count++;
	return ToIsrCriticalRegionSavedLevel(g_critical_isr_count);
}

extern void RLM3_ExitCritical()
{
	if (SIM_RLM3_Is_IRQ())
		FAIL("RLM3_ExitCritical called from an ISR routine.");
	if (g_critical_count == 0)
		FAIL("RLM3_ExitCritical called without a matching call to RLM3_EnterCritical.");
	g_critical_count--;
}

extern void RLM3_ExitCriticalFromISR(uint32_t saved_level)
{
	if (!SIM_RLM3_Is_IRQ())
		FAIL("RLM3_ExitCriticalFromISR called from a non-ISR routine.");
	if (g_critical_isr_count == 0)
		FAIL("RLM3_ExitCriticalFromISR called without a matching call to RLM3_EnterCriticalFromISR.");
	if (saved_level != ToIsrCriticalRegionSavedLevel(g_critical_isr_count))
		FAIL("RLM3_ExitCriticalFromISR not called with the correct saved level.");
	g_critical_isr_count--;
}

extern bool SIM_IsInCriticalSection()
{
	return (g_critical_count != 0 || g_critical_isr_count != 0);

}


TEST_TEARDOWN(TASK_TEARDOWN)
{
	g_current_time = 0;
	g_is_task_active = false;
	g_critical_count = 0;
	g_critical_isr_count = 0;
}

TEST_FINISH(TASK_FINISH)
{
	if (g_critical_count != 0)
		FAIL("Test ended while in a critical region. (%zd)", g_critical_count);
	if (g_critical_isr_count != 0)
		FAIL("Test ended while in an ISR critical region. (%zd)", g_critical_isr_count);
}
