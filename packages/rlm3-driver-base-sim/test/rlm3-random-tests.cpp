#include "Test.hpp"
#include "rlm3-random.h"
#include <algorithm>


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
	uint8_t buffer[32 * 1024] = { 0 };

	RLM3_Random_Init();
	RLM3_Random_Get(buffer, sizeof(buffer));
	RLM3_Random_Deinit();

	std::sort(buffer, buffer + sizeof(buffer));

	for (size_t i = 0; i < sizeof(buffer); i++)
	{
		double percent = 1.0 * i / (sizeof(buffer) - 1);
		double min = (percent - 0.05) * 255;
		double max = (percent + 0.05) * 255;
		ASSERT(min <= buffer[i] && buffer[i] <= max);
	}
}

TEST_CASE(Random_Get_NotInitialized)
{
	uint8_t buffer[32 * 1024] = { 0 };

	ASSERT_ASSERTS(RLM3_Random_Get(buffer, sizeof(buffer)));
}

TEST_CASE(Random_Get_AddedValues)
{
	SIM_Random_Add((const uint8_t*)"ABCD", 4);
	RLM3_Random_Init();

	uint8_t buffer[6] = {};
	RLM3_Random_Get(buffer, 6);

	ASSERT(buffer[0] == 'A');
	ASSERT(buffer[1] == 'B');
	ASSERT(buffer[2] == 'C');
	ASSERT(buffer[3] == 'D');
	ASSERT(buffer[4] != 0 || buffer[5] != 0);
}
