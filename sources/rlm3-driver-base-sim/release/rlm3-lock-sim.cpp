#include "rlm3-lock.h"
#include "rlm3-sim.hpp"
#include "rlm3-task.h"
#include "Test.hpp"


// Sim does not currently support multiple threads, so these locks are easy.

static volatile bool g_is_critical = false;
static volatile bool g_is_critical_isr = false;
static volatile uint32_t g_saved_critical_level = 0;


extern void RLM3_Lock_EnterCritical()
{
	ASSERT(!SIM_IsISR());
	ASSERT(!g_is_critical);
	ASSERT(!g_is_critical_isr);

	g_is_critical = true;
}

extern void RLM3_Lock_ExitCritical()
{
	ASSERT(!SIM_IsISR());
	ASSERT(g_is_critical);
	ASSERT(!g_is_critical_isr);

	g_is_critical = false;
}

extern uint32_t RLM3_Lock_EnterCriticalISR()
{
	ASSERT(SIM_IsISR());
	ASSERT(!g_is_critical);
	ASSERT(!g_is_critical_isr);

	g_is_critical_isr = true;

	g_saved_critical_level *= 55987;
	g_saved_critical_level += 40973;

	return g_saved_critical_level;
}

extern void RLM3_Lock_ExitCriticalISR(uint32_t saved_level)
{
	ASSERT(SIM_IsISR());
	ASSERT(!g_is_critical);
	ASSERT(g_is_critical_isr);
	ASSERT(saved_level == g_saved_critical_level);

	g_is_critical_isr = false;
}

extern uint32_t RLM3_Lock_EnterCriticalSafe()
{
	if (RLM3_Task_IsISR())
		return RLM3_Lock_EnterCriticalISR();
	RLM3_Lock_EnterCritical();
	return 0;
}

extern void RLM3_Lock_ExitCriticalSafe(uint32_t saved_level)
{
	if (RLM3_Task_IsISR())
		RLM3_Lock_ExitCriticalISR(saved_level);
	else
		RLM3_Lock_ExitCritical();
}

extern void RLM3_SpinLock_Init(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_IsISR());

	lock->is_locked = false;
}

extern void RLM3_SpinLock_Deinit(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!lock->is_locked);
}

extern void RLM3_SpinLock_Enter(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!lock->is_locked);

	lock->is_locked = true;
}

extern bool RLM3_SpinLock_Try(RLM3_SpinLock* lock, size_t timeout_ms)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!lock->is_locked);

	if (lock->is_locked)
		return false;

	lock->is_locked = true;
	return true;
}

extern void RLM3_SpinLock_Leave(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_IsISR());
	ASSERT(lock->is_locked);

	lock->is_locked = false;
}


extern void RLM3_MutexLock_Init(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_IsISR());

	lock->is_locked = false;
}

extern void RLM3_MutexLock_Deinit(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!lock->is_locked);
}

extern void RLM3_MutexLock_Enter(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!lock->is_locked);

	lock->is_locked = true;
}

extern bool RLM3_MutexLock_Try(RLM3_MutexLock* lock, size_t timeout_ms)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!lock->is_locked);

	if (lock->is_locked)
		return false;

	lock->is_locked = true;
	return true;
}

extern void RLM3_MutexLock_Leave(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_IsISR());
	ASSERT(lock->is_locked);

	lock->is_locked = false;
}

extern bool SIM_IsInCritical()
{
	return (g_is_critical || g_is_critical_isr);
}

TEST_TEARDOWN(SIM_Lock_Teardown)
{
	g_is_critical = false;
	g_is_critical_isr = false;
}

TEST_FINISH(TASK_FINISH)
{
	if (g_is_critical != 0)
		FAIL("Test ended while in a critical region.");
	if (g_is_critical_isr != 0)
		FAIL("Test ended while in an ISR critical region.");
}

TEST_FINISH(SIM_Lock_Finish)
{
	ASSERT(!g_is_critical && !g_is_critical_isr);
}
