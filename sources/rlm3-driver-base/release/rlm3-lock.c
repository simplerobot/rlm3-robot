#include "rlm3-lock.h"
#include "rlm3-task.h"
#include "Assert.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


#ifdef TEST
static volatile bool g_is_in_critical = false;
static volatile bool g_is_in_critical_isr = false;
static volatile uint32_t g_saved_critical_level = 0;
#endif


extern void RLM3_Lock_EnterCritical()
{
#ifdef TEST
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(!g_is_in_critical && !g_is_in_critical_isr);
	ASSERT(RLM3_Task_IsSchedulerRunning());

#endif

	taskDISABLE_INTERRUPTS();

#ifdef TEST
	ASSERT(!g_is_in_critical && !g_is_in_critical_isr);
	g_is_in_critical = true;
#endif
}

extern void RLM3_Lock_ExitCritical()
{
#ifdef TEST
	ASSERT(g_is_in_critical && !g_is_in_critical_isr);
	g_is_in_critical = false;
#endif

	taskENABLE_INTERRUPTS();

#ifdef TEST
	ASSERT(!g_is_in_critical && !g_is_in_critical_isr);
#endif
}

extern uint32_t RLM3_Lock_EnterCriticalISR()
{
#ifdef TEST
	ASSERT(RLM3_Task_IsISR());
	ASSERT(!g_is_in_critical && !g_is_in_critical_isr);
#endif

	uint32_t saved_level = taskENTER_CRITICAL_FROM_ISR();

#ifdef TEST
	ASSERT(!g_is_in_critical && !g_is_in_critical_isr);
	g_is_in_critical_isr = true;
	g_saved_critical_level = saved_level;
#endif

	return saved_level;
}

extern void RLM3_Lock_ExitCriticalISR(uint32_t saved_level)
{
#ifdef TEST
	ASSERT(!g_is_in_critical && g_is_in_critical_isr);
	ASSERT(saved_level == g_saved_critical_level);
#endif

	taskEXIT_CRITICAL_FROM_ISR(saved_level);

#ifdef TEST
	ASSERT(!g_is_in_critical && !g_is_in_critical_isr);
#endif
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

static bool Atomic_SetBool(volatile bool* value)
{
	bool old_value;

	RLM3_Lock_EnterCritical();
	old_value = *value;
	*value = true;
	RLM3_Lock_ExitCritical();

	return old_value;
}

extern void RLM3_SpinLock_Init(RLM3_SpinLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	ASSERT(!lock->is_initialized);
	lock->owner = NULL;
	lock->is_initialized = true;
#endif

	lock->is_locked = false;
}

extern void RLM3_SpinLock_Deinit(RLM3_SpinLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
	ASSERT(!lock->is_locked);
	ASSERT(lock->owner == NULL);
	lock->is_initialized = false;
#endif
}

extern void RLM3_SpinLock_Enter(RLM3_SpinLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
#endif

	while (Atomic_SetBool(&lock->is_locked))
	{
		RLM3_Task_Yield();
		if (lock->is_locked)
			RLM3_Task_Delay(0);
	}

#ifdef TEST
	ASSERT(lock->owner == NULL);
	lock->owner = RLM3_Task_GetCurrent();
#endif
}

extern bool RLM3_SpinLock_Try(RLM3_SpinLock* lock, size_t timeout_ms)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
#endif

	RLM3_Time start_time = RLM3_Time_Get();
	while (Atomic_SetBool(&lock->is_locked))
	{
		if (RLM3_Time_Get() - start_time >= timeout_ms)
			return false;

		RLM3_Task_Yield();
		if (lock->is_locked)
			RLM3_Task_Delay(0);
	}

#ifdef TEST
	ASSERT(lock->owner == NULL);
	lock->owner = RLM3_Task_GetCurrent();
#endif

	return true;
}

extern void RLM3_SpinLock_Leave(RLM3_SpinLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
	ASSERT(lock->is_locked);
	ASSERT(lock->owner == RLM3_Task_GetCurrent());
	lock->owner = NULL;
#endif

	lock->is_locked = false;
}

#if configNUM_THREAD_LOCAL_STORAGE_POINTERS != 1
#error "Mutex Locks require a thread local storage element to maintain the waiting list"
#endif

static void SetNext(RLM3_Task task, RLM3_Task next)
{
	vTaskSetThreadLocalStoragePointer((TaskHandle_t)task, 0, next);
}

static RLM3_Task GetNext(RLM3_Task task)
{
	return (RLM3_Task)pvTaskGetThreadLocalStoragePointer((TaskHandle_t)task, 0);
}

static void AtomicAddTaskToWaitQueue(RLM3_MutexLock* lock, RLM3_Task task)
{
	RLM3_Lock_EnterCritical();
	SetNext(task, lock->queue);
	lock->queue = task;
	RLM3_Lock_ExitCritical();
}

static void AtomicRemoveTaskFromWaitQueue(RLM3_MutexLock* lock, RLM3_Task task, RLM3_Task* prev_out, RLM3_Task* next_out)
{
	// Walk down the linked list to find the target task.  This assumes a short queue since this must not disable interrupts for long.
	RLM3_Task prev = NULL;
	RLM3_Lock_EnterCritical();
	RLM3_Task cursor = lock->queue;
	RLM3_Task next = GetNext(cursor);
	while (cursor != NULL && cursor != task)
	{
		prev = cursor;
		cursor = next;
		next = GetNext(cursor);
	}
	if (cursor != NULL)
	{
		if (prev != NULL)
			SetNext(prev, next);
		else
			lock->queue = next;
	}
	RLM3_Lock_ExitCritical();

	ASSERT(cursor != NULL);

	SetNext(task, NULL);

	if (prev_out != NULL)
		*prev_out = prev;
	if (next_out != NULL)
		*next_out = next;
}

extern void RLM3_MutexLock_Init(RLM3_MutexLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(RLM3_Task_IsSchedulerRunning());
	ASSERT(!lock->is_initialized);
	lock->owner = NULL;
	lock->is_initialized = true;
#endif

	lock->queue = NULL;
}

extern void RLM3_MutexLock_Deinit(RLM3_MutexLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
	ASSERT(lock->owner == NULL);
	ASSERT(lock->queue == NULL);
	lock->is_initialized = false;
#endif

	lock->queue = NULL;
}

extern void RLM3_MutexLock_Enter(RLM3_MutexLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
	ASSERT(lock->owner != RLM3_Task_GetCurrent());
#endif

	// Make sure this is a valid active task.
	RLM3_Task current_task = RLM3_Task_GetCurrent();
	AtomicAddTaskToWaitQueue(lock, current_task);

	// Wait until no other tasks are in front of us.
	RLM3_Task blocking_task = GetNext(current_task);
	while (blocking_task != NULL)
	{
		RLM3_Task_Take();
		blocking_task = GetNext(current_task);
	}

#ifdef TEST
	ASSERT(lock->owner == NULL);
	lock->owner = RLM3_Task_GetCurrent();
#endif
}

extern bool RLM3_MutexLock_Try(RLM3_MutexLock* lock, size_t timeout_ms)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
	ASSERT(lock->owner != RLM3_Task_GetCurrent());
#endif

	// Make sure this is a valid active task.
	RLM3_Task current_task = RLM3_Task_GetCurrent();
	AtomicAddTaskToWaitQueue(lock, current_task);

	// Wait until no other tasks are in front of us.
	RLM3_Time start_time = RLM3_Time_Get();
	RLM3_Task blocking_task = GetNext(current_task);
	while (blocking_task != NULL)
	{
		if (!RLM3_Task_TakeUntil(start_time, timeout_ms))
		{
			// We timed out.  Remove our task from the waiting queue.
			RLM3_Task prev = NULL;
			RLM3_Task next = NULL;
			AtomicRemoveTaskFromWaitQueue(lock, current_task, &prev, &next);

			// If we actually may have gotten a notification, pass it on to the new candidate.
			if (next == NULL && prev != NULL)
				RLM3_Task_Give(prev);
			return false;
		}
		blocking_task = GetNext(current_task);
	}


#ifdef TEST
	ASSERT(lock->owner == NULL);
	lock->owner = RLM3_Task_GetCurrent();
#endif

	return true;
}

extern void RLM3_MutexLock_Leave(RLM3_MutexLock* lock)
{
#ifdef TEST
	ASSERT(lock);
	ASSERT(!RLM3_Task_IsISR());
	ASSERT(lock->is_initialized);
	ASSERT(lock->owner == RLM3_Task_GetCurrent());
	ASSERT(lock->queue != NULL);
	lock->owner = NULL;
#endif

	// Remove this task from the lock's queue.
	RLM3_Task current_task = RLM3_Task_GetCurrent();
	RLM3_Task blocked_task = NULL;
	AtomicRemoveTaskFromWaitQueue(lock, current_task, &blocked_task, NULL);

	// Wake up the next task in the queue.
	RLM3_Task_Give(blocked_task);
}

