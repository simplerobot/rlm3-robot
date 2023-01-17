#include "Test.hpp"
#include "rlm3-id.h"
#include <random>


TEST_CASE(GetUniqueDeviceId_HappyCase)
{
	uint8_t id[12];
	for (size_t i = 0; i < 12; i++)
		id[i] = RLM3_ID_Get(i);

	uint8_t expected[12] = { 0x39, 0x00, 0x3d, 0x00, 0x11, 0x51, 0x36, 0x30, 0x34, 0x38, 0x37, 0x30 };
	for (size_t i = 0; i < 12; i++)
		ASSERT(id[i] == expected[i]);
}

TEST_CASE(GetUniqueDeviceId_InvalidIndex)
{
	ASSERT_ASSERTS(RLM3_ID_Get(12));
}

TEST_CASE(SetUniqueDeviceId_HappyCase)
{
	for (size_t i = 0; i < 12; i++)
		SIM_ID_Set(i, 1 + i);

	for (size_t i = 0; i < 12; i++)
		ASSERT(RLM3_ID_Get(i) == 1 + i);
}
