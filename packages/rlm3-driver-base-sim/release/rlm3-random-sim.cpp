#include "rlm3-random.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"
#include <queue>
#include <random>


static bool g_is_initialized = false;


extern void RLM3_Random_Init()
{
	ASSERT(!g_is_initialized);
	g_is_initialized = true;
}

extern void RLM3_Random_Deinit()
{
	ASSERT(g_is_initialized);
	g_is_initialized = false;
}

extern bool RLM3_Random_IsInit()
{
	return g_is_initialized;
}

extern void SIM_Random_Add(uint32_t entropy)
{
	SIM_AddInterrupt([=]() {
		RLM3_Random_CB_ISR(entropy);
	});
}

extern __attribute__((weak)) void RLM3_Random_CB_ISR(uint32_t entropy)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

TEST_TEARDOWN(SIM_Random_Teardown)
{
	g_is_initialized = false;
}
