#include "Test.hpp"
#include "rlm3-base.h"
#include <random>


TEST_CASE(GetUniqueDeviceId_HappyCase)
{
	uint8_t id[12];
	RLM3_GetUniqueDeviceId(id);
}

TEST_CASE(SetUniqueDeviceId_HappyCase)
{
	uint8_t id_in[12];
	for (size_t i = 0; i < 12; i++)
		id_in[i] = i;
	SIM_SetUniqueDeviceId(id_in);

	uint8_t id_out[12];
	RLM3_GetUniqueDeviceId(id_out);
	for (size_t i = 0; i < 12; i++)
		ASSERT(id_out[i] == i);
}

TEST_CASE(GetUniqueDeviceShortId_HappyCase)
{
	uint32_t id = RLM3_GetUniqueDeviceShortId();

	ASSERT(id != 0);
}

TEST_CASE(GetUniqueDeviceShortId_Distribution)
{
	size_t distributions[8][16] = { 0 };
	std::default_random_engine random;

	for (size_t i = 0; i < 100000; i++)
	{
		uint8_t id_in[12];
		for (size_t i = 0; i < 12; i++)
			id_in[i] = random();
		SIM_SetUniqueDeviceId(id_in);

		uint32_t id = RLM3_GetUniqueDeviceShortId();

		for (size_t j = 0; j < 8; j++)
			distributions[j][(id >> (4 * j))&0x0F]++;
	}

	size_t min = 100000 / 16 * 9 / 10;
	size_t max = 100000 / 16 * 11 / 10;
	for (size_t i = 0; i < 8; i++)
		for (size_t j = 0; j < 16; j++)
			ASSERT(min < distributions[i][j] && distributions[i][j] < max);
}

TEST_CASE(GetUniqueDeviceShortId_Set)
{
	SIM_SetUniqueDeviceShortId(0x12345678);

	uint32_t id = RLM3_GetUniqueDeviceShortId();

	ASSERT(id == 0x12345678);
}

TEST_CASE(DebugOutput_HappyCase)
{
	SIM_ExpectDebugOutput("abc");

	RLM3_DebugOutput('a');
	RLM3_DebugOutput('b');
	RLM3_DebugOutput('c');
}

TEST_CASE(DebugOutput_None)
{
	SIM_ExpectDebugOutput("");
}

TEST_CASE(DebugOutput_NonMatching)
{
	auto test = [] {
		SIM_ExpectDebugOutput("abc");

		RLM3_DebugOutput('a');
		RLM3_DebugOutput('d');
		RLM3_DebugOutput('c');
	};

	TestCaseListItem test_case(test, "test", __FILE__, __LINE__);
	ASSERT(!test_case.Run());
}

TEST_CASE(DebugOutput_Missing)
{
	auto test = [] {
		SIM_ExpectDebugOutput("abc");

		RLM3_DebugOutput('a');
		RLM3_DebugOutput('b');
	};

	TestCaseListItem test_case(test, "test", __FILE__, __LINE__);
	ASSERT(!test_case.Run());
}

TEST_CASE(DebugOutput_Extra)
{
	auto test = [] {
		SIM_ExpectDebugOutput("abc");

		RLM3_DebugOutput('a');
		RLM3_DebugOutput('b');
		RLM3_DebugOutput('c');
		RLM3_DebugOutput('d');
	};

	TestCaseListItem test_case(test, "test", __FILE__, __LINE__);
	ASSERT(!test_case.Run());
}




