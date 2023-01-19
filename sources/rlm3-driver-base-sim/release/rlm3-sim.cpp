#include "rlm3-sim.hpp"
#include "rlm3-task.h"
#include "rlm3-lock.h"
#include <queue>
#include <utility>
#include "Test.hpp"
#include <sstream>


struct Handler
{
	Handler(RLM3_Time delay, std::function<void()> handler, bool is_interrupt)
		: delay(delay)
		, handler(handler)
		, is_interrupt(is_interrupt)
	{
	}

	RLM3_Time delay;
	std::function<void()> handler;
	bool is_interrupt;
};

static bool g_is_in_interrupt_handler = false;
static bool g_is_in_secondary_task = false;
static std::queue<Handler> g_sim_handler_queue;
static RLM3_Time g_last_handler_time = 0;
static RLM3_Time g_next_delay = 0;


extern bool SIM_IsISR()
{
	return g_is_in_interrupt_handler;
}

extern bool SIM_IsMainTask()
{
	return !g_is_in_secondary_task;
}

extern void SIM_AddInterrupt(std::function<void()> interrupt)
{
	ASSERT(!SIM_IsISR());
	g_sim_handler_queue.emplace(g_next_delay, interrupt, true);
	g_next_delay = 0;
}

extern void SIM_AddTask(std::function<void()> task)
{
	ASSERT(!SIM_IsISR());
	g_sim_handler_queue.emplace(g_next_delay, task, false);
	g_next_delay = 0;
}

extern void SIM_AddDelay(RLM3_Time delay)
{
	ASSERT(!SIM_IsISR());
	g_next_delay += delay;
}

extern bool SIM_HasNextHandler()
{
	ASSERT(!SIM_IsISR());
	return !g_sim_handler_queue.empty();
}

extern RLM3_Time SIM_GetNextHandlerTime()
{
	ASSERT(!SIM_IsISR());
	ASSERT(!g_sim_handler_queue.empty());
	RLM3_Time current_time = RLM3_Time_Get();
	RLM3_Time target_time = g_last_handler_time + g_sim_handler_queue.front().delay;
	return std::max(current_time, target_time);
}

extern void SIM_DoInterrupt(std::function<void()> interrupt)
{
	if (SIM_IsInCritical())
		FAIL("Trying to execute an interrupt while in a critical section.");
	if (SIM_IsISR())
		FAIL("Trying to execute an interrupt while already in an interrupt.");
	if (!SIM_IsMainTask())
		FAIL("Trying to execute an interrupt while in a secondary task.");

	try
	{
		g_is_in_interrupt_handler = true;
		interrupt();
		g_is_in_interrupt_handler = false;
	}
	catch (...)
	{
		g_is_in_interrupt_handler = false;
		throw;
	}

	if (SIM_IsInCritical())
		FAIL("Interrupt handler finished while in a critical section.");
	if (SIM_IsISR())
		FAIL("Interrupt handler finished while already in an interrupt.");
	if (!SIM_IsMainTask())
		FAIL("Interrupt handler finished while in a secondary task.");
}

extern void SIM_DoTask(std::function<void()> task)
{
	if (SIM_IsInCritical())
		FAIL("Trying to execute a task while in a critical section.");
	if (SIM_IsISR())
		FAIL("Trying to execute a task while already in an interrupt.");
	if (!SIM_IsMainTask())
		FAIL("Trying to execute a task while in a secondary task.");

	try
	{
		g_is_in_secondary_task = true;
		task();
		g_is_in_secondary_task = false;
	}
	catch (...)
	{
		g_is_in_secondary_task = false;
		throw;
	}

	if (SIM_IsInCritical())
		FAIL("Task handler finished while in a critical section.");
	if (SIM_IsISR())
		FAIL("Task handler finished while already in an interrupt.");
	if (!SIM_IsMainTask())
		FAIL("Task handler finished while in a secondary task.");
}

extern void SIM_RunNextHandler()
{
	ASSERT(!SIM_IsISR());
	ASSERT(!g_sim_handler_queue.empty());
	ASSERT(!SIM_IsInCritical());
	g_last_handler_time = RLM3_Time_Get();

	auto handler = g_sim_handler_queue.front();
	g_sim_handler_queue.pop();

	if (handler.is_interrupt)
		SIM_DoInterrupt(handler.handler);
	else
		SIM_DoTask(handler.handler);
}


TEST_SETUP(SIMULATOR_INITIALIZATION)
{
	g_is_in_interrupt_handler = false;
	while (!g_sim_handler_queue.empty())
		g_sim_handler_queue.pop();
	g_last_handler_time = 0;
	g_next_delay = 0;
}

TEST_FINISH(SIMULATOR_VALIDATE)
{
	if (!g_sim_handler_queue.empty())
		FAIL("Simulator did not use all task/interrupt events.");
}

extern std::string SIM_SafeString(const std::string& input)
{
	std::ostringstream out;
	for (char c : input)
	{
		if (c == '\\')
		{
			out.put('\\');
			out.put('\\');
		}
		else if (' ' <= c && c <= '~')
		{
			out.put(c);
		}
		else if (c == '\r')
		{
			out.put('\\');
			out.put('r');
		}
		else if (c == '\n')
		{
			out.put('\\');
			out.put('n');
		}
		else
		{
			out.put('\\');
			out.put('x');
			static const char* k_hex_digits = "0123456789ABCDEF";
			out.put(k_hex_digits[(c >> 4) & 0x0F]);
			out.put(k_hex_digits[(c >> 0) & 0x0F]);
		}
	}
	return out.str();
}
