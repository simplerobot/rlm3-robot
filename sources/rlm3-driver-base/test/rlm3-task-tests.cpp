#include "Test.h"
#include "rlm3-task.h"
#include "rlm3-lock.h"
#include "main.h"
#include <algorithm>


TEST_CASE(Task_GetCurrentTime_HappyCase)
{
	RLM3_Time time0 = RLM3_Time_Get();
	size_t steps_0 = 0;
	while (steps_0 < 1000000000 && RLM3_Time_Get() == time0)
		steps_0++;
	RLM3_Time time1 = RLM3_Time_Get();
	size_t steps_1 = 0;
	while (steps_1 < 1000000000 && RLM3_Time_Get() == time1)
		steps_1++;
	RLM3_Time time2 = RLM3_Time_Get();
	size_t steps_2 = 0;
	while (steps_2 < 1000000000 && RLM3_Time_Get() == time2)
		steps_2++;

	ASSERT(time1 == time0 + 1);
	ASSERT(time2 == time1 + 1);
	size_t steps_max = std::max(steps_1, steps_2);
	size_t steps_min = std::min(steps_1, steps_2);
	ASSERT(1.0 * steps_min / steps_max > 0.99);
}

TEST_CASE(Task_Create_HappyCase)
{
	static volatile bool success = false;
	auto thread_fn = []()
	{
		success = true;
	};

	RLM3_Task result = RLM3_Task_Create(thread_fn, 128, "test");

	ASSERT(result != NULL);
	ASSERT(!success);
	RLM3_Task_Yield();
	ASSERT(success);
}

TEST_CASE(Task_Yield_SingleThread)
{
	RLM3_Time start_time = RLM3_Time_Get();
	for (size_t i = 0; i < 10; i++)
		RLM3_Task_Yield();
	RLM3_Time end_time = RLM3_Time_Get();

	ASSERT((end_time - start_time) <= 1);
}

TEST_CASE(Task_Yield_HappyCase)
{
	static volatile size_t count = 0;
	auto thread_fn = []()
	{
		for (size_t i = 0; i < 10; i++)
		{
			count++;
			RLM3_Task_Yield();
		}
	};

	RLM3_Task_Create(thread_fn, 128, "test");

	RLM3_Time start_time = RLM3_Time_Get();
	for (size_t i = 0; i < 10; i++)
	{
		ASSERT(count == i);
		RLM3_Task_Yield();
	}
	RLM3_Time end_time = RLM3_Time_Get();

	ASSERT((end_time - start_time) <= 1);
}

TEST_CASE(Task_Delay_HappyCase)
{
	RLM3_Time time0 = RLM3_Time_Get();
	RLM3_Task_Delay(1);
	RLM3_Time time1 = RLM3_Time_Get();
	RLM3_Task_Delay(1);
	RLM3_Time time2 = RLM3_Time_Get();
	RLM3_Task_Delay(1);
	RLM3_Time time3 = RLM3_Time_Get();

	ASSERT(time0 + 1 <= time1 && time1 <= time0 + 3);
	ASSERT(time1 + 1 <= time2 && time2 <= time1 + 2);
	ASSERT(time2 + 1 <= time3 && time3 <= time2 + 2);
}

TEST_CASE(Task_DelayUntil_HappyCase)
{
	RLM3_Time start_time = RLM3_Time_Get();
	RLM3_Task_DelayUntil(start_time, 3);
	RLM3_Time actual_time = RLM3_Time_Get();

	ASSERT(start_time + 3 == actual_time);
}

TEST_CASE(Task_GetCurrentTask_SingleThread)
{
	RLM3_Task task = RLM3_Task_GetCurrent();
	ASSERT(task != nullptr);
	ASSERT(task == RLM3_Task_GetCurrent());
}

TEST_CASE(Task_GetCurrentTask_MultipleThreads)
{
	static volatile size_t g_thread_count = 0;
	static RLM3_Task g_tasks[6] = {};
	auto thread_fn = []()
	{
		RLM3_Lock_EnterCritical();
		size_t id = g_thread_count++;
		RLM3_Lock_ExitCritical();

		ASSERT(id < 6);
		if (id < 6)
			g_tasks[id] = RLM3_Task_GetCurrent();
	};

	RLM3_Task tasks[6];
	for (size_t i = 0; i < 6; i++)
		tasks[i] = RLM3_Task_Create(thread_fn, 128, "test");
	RLM3_Task_Delay(1);

	RLM3_Task current_task = RLM3_Task_GetCurrent();
	for (size_t i = 0; i < 6; i++)
	{
		ASSERT(g_tasks[i] != nullptr);
		ASSERT(g_tasks[i] != current_task);
		for (size_t j = i + 1; j < 6; j++)
			ASSERT(g_tasks[i] != g_tasks[j]);
		bool found = false;
		for (size_t j = 0; j < 6; j++)
			if (g_tasks[i] == tasks[j])
				found = true;
		ASSERT(found);
	}
}

TEST_CASE(Task_Give_HappyCase)
{
	RLM3_Task_Give(RLM3_Task_GetCurrent());

	RLM3_Task_Take();
}

TEST_CASE(Task_Give_SecondThread)
{
	static volatile size_t g_count = 0;
	auto thread_fn = []()
	{
		for (size_t i = 0; i < 5; i++)
		{
			RLM3_Task_Take();
			g_count++;
		}
	};

	RLM3_Task task = RLM3_Task_Create(thread_fn, 128, "test");

	RLM3_Time start_time = RLM3_Time_Get();
	for (size_t i = 0; i < 5; i++)
	{
		RLM3_Task_Yield();
		ASSERT(g_count == i);
		RLM3_Task_Give(task);
		RLM3_Task_Yield();
	}
	RLM3_Time end_time = RLM3_Time_Get();

	ASSERT((end_time - start_time) <= 1);
}

TEST_CASE(Task_Give_Multiple)
{
	static volatile size_t g_count = 0;
	auto thread_fn = []()
	{
		for (size_t i = 0; i < 2; i++)
		{
			RLM3_Task_Take();
			g_count++;
		}
	};

	RLM3_Task task = RLM3_Task_Create(thread_fn, 128, "test");

	RLM3_Task_Delay(0); // Make sure we don't have a forced task switch between the next two lines.
	RLM3_Task_Give(task);
	RLM3_Task_Give(task);
	RLM3_Task_Yield();
	RLM3_Task_Yield();
	ASSERT(g_count == 1);
	RLM3_Task_Give(task);
	RLM3_Task_Yield();
	ASSERT(g_count == 2);
}

/*
TEST_CASE(Task_Give_FromISR)
{
	static RLM3_Task g_target_task = RLM3_Task_GetCurrent();
	SetTimer2Callback([] { RLM3_Task_GiveISR(g_target_task); });
	RLM3_Timer2_Init(5000);

	RLM3_Time start_time = RLM3_GetCurrentTime();
	for (size_t i = 0; i < 100; i++)
		RLM3_Task_Take();
	RLM3_Time end_time = RLM3_GetCurrentTime();

	RLM3_Timer2_Deinit();

	RLM3_Time elapsed = end_time - start_time;
	ASSERT(20 <= elapsed && elapsed <= 21);
}

TEST_CASE(Task_TakeWithTimeout_HappyCase)
{
	RLM3_Task_Give(RLM3_Task_GetCurrent());

	RLM3_Time start_time = RLM3_GetCurrentTime();
	bool result = RLM3_Task_TakeWithTimeout(10);
	RLM3_Time elapsed = RLM3_GetCurrentTime() - start_time;

	ASSERT(result);
	ASSERT(elapsed <= 1);
}

TEST_CASE(Task_TakeWithTimeout_Timeout)
{
	RLM3_Time start_time = RLM3_GetCurrentTime();
	bool result = RLM3_Task_TakeWithTimeout(10);
	RLM3_Time elapsed = RLM3_GetCurrentTime() - start_time;

	ASSERT(!result);
	ASSERT(10 <= elapsed && elapsed <= 11);
}

TEST_CASE(Task_TakeUntil_HappyCase)
{
	RLM3_Task_Give(RLM3_Task_GetCurrent());
	RLM3_Time start_time = RLM3_GetCurrentTime();

	bool result = RLM3_Task_TakeUntil(start_time, 10);
	RLM3_Time elapsed = RLM3_GetCurrentTime() - start_time;

	ASSERT(result);
	ASSERT(elapsed <= 1);
}

TEST_CASE(Task_TakeUntil_Timeout)
{
	RLM3_Time start_time = RLM3_GetCurrentTime();
	RLM3_Time target_time = start_time + 10;

	bool result = RLM3_Task_TakeUntil(start_time, 10);
	RLM3_Time end_time = RLM3_GetCurrentTime();

	ASSERT(!result);
	ASSERT(end_time == target_time);
}

TEST_CASE(Task_EnterCritical)
{
	static volatile uint32_t g_count = 0;
	SetTimer2Callback([] { ASSERT(g_count % 1000 == 0); });
	RLM3_Timer2_Init(5000);

	for (size_t i = 0; i < 1000; i++)
	{
		RLM3_Lock_EnterCritical();
		for (size_t j = 0; j < 1000; j++)
			g_count++;
		RLM3_Lock_ExitCritical();
	}

	RLM3_Timer2_Deinit();
}

TEST_CASE(Task_EnterCriticalFromISR)
{
	SetTimer2Callback([] {
		uint32_t saved = RLM3_Lock_EnterCriticalISR();
		RLM3_Lock_ExitCriticalISR(saved);
	});
	RLM3_Timer2_Init(5000);
	RLM3_Task_Delay(10);
	RLM3_Timer2_Deinit();
}

static void StartTimer(size_t frequency_hz)
{
}

static void StopTimer()
{
}

*/
