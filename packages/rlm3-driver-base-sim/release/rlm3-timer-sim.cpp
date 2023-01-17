#include "rlm3-timer.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"


static bool g_timer2_is_initialized = false;

extern void RLM3_Timer2_Init(size_t frequency_hz)
{
	ASSERT(1 <= frequency_hz && frequency_hz <= 180000);
	ASSERT(!g_timer2_is_initialized);
	g_timer2_is_initialized = true;
}

extern void RLM3_Timer2_Deinit()
{
	ASSERT(g_timer2_is_initialized);
	g_timer2_is_initialized = false;
}

extern bool RLM3_Timer2_IsInit()
{
	return g_timer2_is_initialized;
}

extern __attribute__((weak)) void RLM3_Timer2_Event_CB_ISR()
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern void SIM_Timer2_Event()
{
	SIM_AddInterrupt([] {
		ASSERT(RLM3_Timer2_IsInit());
		RLM3_Timer2_Event_CB_ISR();
	});
}

TEST_START(SIM_TIMER)
{
	g_timer2_is_initialized = false;
}
