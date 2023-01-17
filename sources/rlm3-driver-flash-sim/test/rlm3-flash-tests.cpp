#include "Test.hpp"
#include "rlm3-flash.h"
#include <random>


TEST_CASE(RLM3_Flash_Init_HappyCase)
{
	ASSERT(!RLM3_Flash_IsInit());
	RLM3_Flash_Init();
	ASSERT(RLM3_Flash_IsInit());
}

TEST_CASE(RLM3_Flash_Init_Duplicate)
{
	RLM3_Flash_Init();
	ASSERT_ASSERTS(RLM3_Flash_Init());
}

TEST_CASE(RLM3_Flash_DeInit_HappyCase)
{
	RLM3_Flash_Init();
	RLM3_Flash_Deinit();
	ASSERT(!RLM3_Flash_IsInit());
}

TEST_CASE(RLM3_Flash_DeInit_NeverInitialized)
{
	ASSERT_ASSERTS(RLM3_Flash_Deinit());
}

TEST_CASE(RLM3_Flash_Write_HappyCase)
{
	uint8_t buffer[RLM3_FLASH_SIZE];
	std::default_random_engine rand(20220720);
	for (size_t i = 0; i < RLM3_FLASH_SIZE; i++)
		buffer[i] = (uint8_t)rand();

	RLM3_Flash_Init();
	ASSERT(RLM3_Flash_Write(0, buffer, RLM3_FLASH_SIZE));

	rand.seed(20220720);
	for (size_t i = 0; i < RLM3_FLASH_SIZE; i++)
		ASSERT(SIM_Flash_Read(i) == (uint8_t)rand());
}

TEST_CASE(RLM3_Flash_Write_NotInitialized)
{
	uint8_t buffer[RLM3_FLASH_SIZE];

	ASSERT_ASSERTS(RLM3_Flash_Write(0, buffer, RLM3_FLASH_SIZE));
}

TEST_CASE(RLM3_Flash_Write_Null)
{
	RLM3_Flash_Init();

	ASSERT_ASSERTS(RLM3_Flash_Write(0, nullptr, RLM3_FLASH_SIZE));
}

TEST_CASE(RLM3_Flash_Write_InvalidAddress)
{
	uint8_t buffer[RLM3_FLASH_SIZE];
	RLM3_Flash_Init();

	ASSERT_ASSERTS(RLM3_Flash_Write(RLM3_FLASH_SIZE, buffer, 1));
}

TEST_CASE(RLM3_Flash_Write_InvalidSize)
{
	uint8_t buffer[RLM3_FLASH_SIZE + 1];
	RLM3_Flash_Init();

	ASSERT_ASSERTS(RLM3_Flash_Write(RLM3_FLASH_SIZE, buffer, RLM3_FLASH_SIZE + 1));
}

TEST_CASE(RLM3_Flash_Write_ForcedFailure)
{
	uint8_t buffer[RLM3_FLASH_SIZE];
	SIM_Flash_ForceFailure();

	RLM3_Flash_Init();
	ASSERT(!RLM3_Flash_Write(0, buffer, RLM3_FLASH_SIZE));
}

TEST_CASE(RLM3_Flash_Read_HappyCase)
{
	std::default_random_engine rand(20220720);
	for (size_t i = 0; i < RLM3_FLASH_SIZE; i++)
		SIM_Flash_Write(i, (uint8_t)rand());

	RLM3_Flash_Init();
	uint8_t buffer[RLM3_FLASH_SIZE] = {};
	ASSERT(RLM3_Flash_Read(0, buffer, RLM3_FLASH_SIZE));

	rand.seed(20220720);
	for (size_t i = 0; i < RLM3_FLASH_SIZE; i++)
		ASSERT(buffer[i] == (uint8_t)rand());
}

TEST_CASE(RLM3_Flash_Read_NotInitialized)
{
	uint8_t buffer[RLM3_FLASH_SIZE];

	ASSERT_ASSERTS(RLM3_Flash_Read(0, buffer, RLM3_FLASH_SIZE));
}

TEST_CASE(RLM3_Flash_Read_Null)
{
	RLM3_Flash_Init();

	ASSERT_ASSERTS(RLM3_Flash_Read(0, nullptr, RLM3_FLASH_SIZE));
}

TEST_CASE(RLM3_Flash_Read_InvalidAddress)
{
	uint8_t buffer[RLM3_FLASH_SIZE];
	RLM3_Flash_Init();

	ASSERT_ASSERTS(RLM3_Flash_Read(RLM3_FLASH_SIZE, buffer, 1));
}

TEST_CASE(RLM3_Flash_Read_InvalidSize)
{
	uint8_t buffer[RLM3_FLASH_SIZE + 1];
	RLM3_Flash_Init();

	ASSERT_ASSERTS(RLM3_Flash_Read(RLM3_FLASH_SIZE, buffer, RLM3_FLASH_SIZE + 1));
}

TEST_CASE(RLM3_Flash_Read_ForcedFailure)
{
	uint8_t buffer[RLM3_FLASH_SIZE];
	SIM_Flash_ForceFailure();

	RLM3_Flash_Init();
	ASSERT(!RLM3_Flash_Read(0, buffer, RLM3_FLASH_SIZE));
}

