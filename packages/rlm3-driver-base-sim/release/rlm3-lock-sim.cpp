#include "rlm3-lock.h"
#include "rlm3-sim.hpp"
#include "Assert.h"


// Sim does not currently support multiple threads, so these locks are easy.

extern void RLM3_SpinLock_Init(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	lock->is_locked = false;
}

extern void RLM3_SpinLock_Deinit(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!lock->is_locked);
}

extern void RLM3_SpinLock_Enter(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!lock->is_locked);

	lock->is_locked = true;
}

extern bool RLM3_SpinLock_Try(RLM3_SpinLock* lock, size_t timeout_ms)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!lock->is_locked);

	if (lock->is_locked)
		return false;

	lock->is_locked = true;
	return true;
}

extern void RLM3_SpinLock_Leave(RLM3_SpinLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(lock->is_locked);

	lock->is_locked = false;
}


extern void RLM3_MutexLock_Init(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());

	lock->is_locked = false;
}

extern void RLM3_MutexLock_Deinit(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!lock->is_locked);
}

extern void RLM3_MutexLock_Enter(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!lock->is_locked);

	lock->is_locked = true;
}

extern bool RLM3_MutexLock_Try(RLM3_MutexLock* lock, size_t timeout_ms)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!lock->is_locked);

	if (lock->is_locked)
		return false;

	lock->is_locked = true;
	return true;
}

extern void RLM3_MutexLock_Leave(RLM3_MutexLock* lock)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(lock->is_locked);

	lock->is_locked = false;
}

