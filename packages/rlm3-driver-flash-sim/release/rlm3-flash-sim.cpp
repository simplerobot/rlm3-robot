#include "rlm3-flash.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"
#include <cstring>


static bool g_flash_active;
static uint8_t g_flash_data[RLM3_FLASH_SIZE];
static size_t g_flash_failure_count;


static void ValidateFlashRange(uint32_t flash_address, size_t size)
{
	ASSERT(0 < size && size <= RLM3_FLASH_SIZE);
	ASSERT(flash_address < RLM3_FLASH_SIZE);
	ASSERT(flash_address + size <= RLM3_FLASH_SIZE);
}

extern void RLM3_Flash_Init()
{
	ASSERT(!g_flash_active);
	g_flash_active = true;
}

extern void RLM3_Flash_Deinit()
{
	ASSERT(g_flash_active);
	g_flash_active = false;
}

extern bool RLM3_Flash_IsInit()
{
	return g_flash_active;
}

extern bool RLM3_Flash_Write(uint32_t flash_address, const uint8_t* data, size_t size)
{
	ASSERT(g_flash_active);
	ASSERT(data != nullptr);
	ValidateFlashRange(flash_address, size);

	if (g_flash_failure_count > 0)
	{
		g_flash_failure_count--;
		return false;
	}

	std::memcpy(g_flash_data + flash_address, data, size);
	return true;
}

extern bool RLM3_Flash_Read(uint32_t flash_address, uint8_t* data, size_t size)
{
	ASSERT(g_flash_active);
	ASSERT(data != nullptr);
	ValidateFlashRange(flash_address, size);

	if (g_flash_failure_count > 0)
	{
		g_flash_failure_count--;
		return false;
	}

	std::memcpy(data, g_flash_data + flash_address, size);
	return true;
}

extern void SIM_Flash_Write(uint32_t flash_address, uint8_t data)
{
	ValidateFlashRange(flash_address, 1);
	g_flash_data[flash_address] = data;
}

extern uint8_t SIM_Flash_Read(uint32_t flash_address)
{
	ValidateFlashRange(flash_address, 1);
	return g_flash_data[flash_address];
}

extern void SIM_Flash_ForceFailure()
{
	g_flash_failure_count++;
}

TEST_SETUP(SIM_FLASH_INIT)
{
	g_flash_active = false;
	std::memset(g_flash_data, 0, sizeof(g_flash_data));
	g_flash_failure_count = 0;
}
