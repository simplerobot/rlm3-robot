#include "rlm3-random.h"
#include "Test.hpp"
#include <queue>
#include <random>


static bool g_is_initialized = false;
std::queue<uint8_t> g_data;
std::default_random_engine g_random;


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

extern void RLM3_Random_Get(uint8_t* data, size_t size)
{
	ASSERT(g_is_initialized);
	for (size_t i = 0; i < size; i++)
	{
		if (!g_data.empty())
		{
			data[i] = g_data.front();
			g_data.pop();
		}
		else
		{
			data[i] = (uint8_t)g_random();
		}
	}
}

extern void SIM_Random_Add(const uint8_t* data, size_t size)
{
	for (size_t i = 0; i < size; i++)
		g_data.push(data[i]);
}

TEST_SETUP(SIM_Random)
{
	while (!g_data.empty())
		g_data.pop();
	g_is_initialized = false;
}

