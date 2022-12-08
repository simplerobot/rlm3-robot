#include "Test.hpp"
#include "rlm3-memory.h"
#include <random>


TEST_CASE(MEMORY_Lifecycle)
{
	ASSERT(!RLM3_MEMORY_IsInit());
	RLM3_MEMORY_Init();
	ASSERT(RLM3_MEMORY_IsInit());
	RLM3_MEMORY_Deinit();
	ASSERT(!RLM3_MEMORY_IsInit());
}

TEST_CASE(MEMORY_HappyCase_SingleByte)
{
	RLM3_MEMORY_Init();

	volatile uint32_t* test = (uint32_t*)RLM3_EXTERNAL_MEMORY_ADDRESS;
	*test = 0x12345678;

	ASSERT(*test == 0x12345678);

	RLM3_MEMORY_Deinit();
}

TEST_CASE(MEMORY_HappyCase_WriteSequentialWords)
{
	RLM3_MEMORY_Init();

	std::default_random_engine random(20220804);
	for (size_t i = 0; i < RLM3_EXTERNAL_MEMORY_SIZE; i += 4)
		*(uint32_t*)(RLM3_EXTERNAL_MEMORY_ADDRESS + i) = (uint32_t)random();

	random.seed(20220804);
	for (size_t i = 0; i < RLM3_EXTERNAL_MEMORY_SIZE; i += 4)
		ASSERT(*(const uint32_t*)(RLM3_EXTERNAL_MEMORY_ADDRESS + i) == (uint32_t)random());

	RLM3_MEMORY_Deinit();
}

static size_t LCG_Next(size_t x)
{
	// Use an LCG to generate the each memory address in a "random" order.  x = (a * x + c) mod m
	size_t c = 7919; // Relatively prime to m
	size_t a = 33;
	size_t m = RLM3_EXTERNAL_MEMORY_SIZE;

	return (a * x + c) % m;
}

TEST_CASE(MEMORY_HappyCase_WriteRandomBytes)
{
	RLM3_MEMORY_Init();

	std::default_random_engine random(20220804);
	for (size_t i = 0, x = 1; i < RLM3_EXTERNAL_MEMORY_SIZE; i++, x = LCG_Next(x))
		*(RLM3_EXTERNAL_MEMORY_ADDRESS + x) = (uint8_t)random();

	random.seed(20220804);
	for (size_t i = 0, x = 1; i < RLM3_EXTERNAL_MEMORY_SIZE; i++, x = LCG_Next(x))
		ASSERT(*(RLM3_EXTERNAL_MEMORY_ADDRESS + x) == (uint8_t)random());

	RLM3_MEMORY_Deinit();
}

TEST_CASE(MEMORY_GetMemoryPtrOutsideInit)
{
	ASSERT_ASSERTS(*RLM3_EXTERNAL_MEMORY_ADDRESS = 7);
}

TEST_CASE(MEMORY_UseMemoryAfterDeinit)
{
	// Exception handling through signals is a bit wonky, so we are isolating the failure a bit more.
	auto test = []() {
		RLM3_MEMORY_Init();
		uint8_t* p = RLM3_EXTERNAL_MEMORY_ADDRESS;
		RLM3_MEMORY_Deinit();

		*p = 7;
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(MEMORY_UseMemoryAfterDeinit2)
{
	// Exception handling through signals is a bit wonky, so we are isolating the failure a bit more.
	auto test = []() {
		RLM3_MEMORY_Init();
		uint8_t* p = RLM3_EXTERNAL_MEMORY_ADDRESS;
		RLM3_MEMORY_Deinit();

		*p = 7;
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(MEMORY_UseMemoryAfterDeinit3)
{
	// Exception handling through signals is a bit wonky, so we are isolating the failure a bit more.
	auto test = []() {
		RLM3_MEMORY_Init();
		uint8_t* p = RLM3_EXTERNAL_MEMORY_ADDRESS;
		RLM3_MEMORY_Deinit();

		*p = 7;
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(MEMORY_AccessBeforeRange)
{
	// Exception handling through signals is a bit wonky, so we are isolating the failure a bit more.
	auto test = []() {
		RLM3_MEMORY_Init();
		RLM3_EXTERNAL_MEMORY_ADDRESS[-1] = 7;
		RLM3_MEMORY_Deinit();
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(MEMORY_AccessAfterRange)
{
	// Exception handling through signals is a bit wonky, so we are isolating the failure a bit more.
	auto test = []() {
		RLM3_MEMORY_Init();
		RLM3_EXTERNAL_MEMORY_ADDRESS[RLM3_EXTERNAL_MEMORY_SIZE] = 7;
		RLM3_MEMORY_Deinit();
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}


