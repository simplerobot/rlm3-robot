#include "Test.hpp"
#include "rlm3-random.h"
#include "rlm3-task.h"
#include "rlm3-sim.hpp"
#include <algorithm>


std::vector<uint32_t> g_entropy;

extern void RLM3_Random_CB_ISR(uint32_t entropy)
{
	g_entropy.push_back(entropy);
}

TEST_CASE(Random_Init_HappyCase)
{
	ASSERT(!RLM3_Random_IsInit());
	RLM3_Random_Init();
	ASSERT(RLM3_Random_IsInit());
}

TEST_CASE(Random_Init_DoubleInit)
{
	RLM3_Random_Init();
	ASSERT_ASSERTS(RLM3_Random_Init());
}

TEST_CASE(Random_Deinit_HappyCase)
{
	RLM3_Random_Init();
	RLM3_Random_Deinit();
	ASSERT(!RLM3_Random_IsInit());
}

TEST_CASE(Random_Deinit_NeverInit)
{
	ASSERT_ASSERTS(RLM3_Random_Deinit());
}

TEST_CASE(Random_Get_HappyCase)
{
	SIM_Random_Add(1234);
	SIM_Random_Add(3579);
	SIM_AddInterrupt([]() { SIM_Give(); });

	RLM3_Random_Init();
	while (g_entropy.size() < 2)
		RLM3_Task_Take();
	RLM3_Random_Deinit();

	ASSERT(g_entropy.size() == 2);
	ASSERT(g_entropy[0] == 1234);
	ASSERT(g_entropy[1] == 3579);
}

TEST_SETUP(Random_Setup)
{
	g_entropy.clear();
}
