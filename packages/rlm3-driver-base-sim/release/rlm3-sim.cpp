#include "rlm3-sim.hpp"
#include "rlm3-task.h"
#include <queue>
#include <utility>
#include "Test.hpp"


struct InterruptHandler
{
	InterruptHandler(RLM3_Time delay, std::function<void()> handler)
		: delay(delay)
		, handler(handler)
	{
	}

	RLM3_Time delay;
	std::function<void()> handler;
};

static bool g_is_in_interrupt_handler = false;
static std::queue<InterruptHandler> g_sim_interrupt_queue;
static RLM3_Time g_last_interrupt_time = 0;
static RLM3_Time g_next_delay = 0;


extern bool SIM_RLM3_Is_IRQ()
{
	return g_is_in_interrupt_handler;
}

extern void SIM_AddInterrupt(std::function<void()> interrupt)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	g_sim_interrupt_queue.emplace(g_next_delay, interrupt);
	g_next_delay = 0;
}

extern void SIM_AddDelay(RLM3_Time delay)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	g_next_delay += delay;
}

extern bool SIM_HasNextInterrupt()
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	return !g_sim_interrupt_queue.empty();
}

extern RLM3_Time SIM_GetNextInterruptTime()
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!g_sim_interrupt_queue.empty());
	RLM3_Time current_time = RLM3_GetCurrentTime();
	RLM3_Time target_time = g_last_interrupt_time + g_sim_interrupt_queue.front().delay;
	return std::max(current_time, target_time);
}

extern void SIM_DoInterrupt(std::function<void()> interrupt)
{
	if (SIM_IsInCriticalSection())
		FAIL("Trying to execute an interrupt while in a critical section.");

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

	if (SIM_IsInCriticalSection())
		FAIL("Interrupt handler finished while in a critical section.");
}

extern void SIM_RunNextInterrupt()
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!g_sim_interrupt_queue.empty());
	g_last_interrupt_time = RLM3_GetCurrentTime();

	auto handler = g_sim_interrupt_queue.front().handler;
	g_sim_interrupt_queue.pop();
	SIM_DoInterrupt(handler);
}


TEST_SETUP(SIMULATOR_INITIALIZATION)
{
	g_is_in_interrupt_handler = false;
	while (!g_sim_interrupt_queue.empty())
		g_sim_interrupt_queue.pop();
	g_last_interrupt_time = 0;
	g_next_delay = 0;
}

TEST_FINISH(SIMULATOR_VALIDATE)
{
	if (!g_sim_interrupt_queue.empty())
		FAIL("Simulator did not use all interrupt events.");
}
