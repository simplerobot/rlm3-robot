#include "Test.hpp"
#include "rlm3-settings.h"
#include "rlm3-flash.h"
#include "rlm3-memory.h"
#include <cstdio>


TEST_CASE(FlashLayout_Size)
{
	std::printf("Size: %zd Diff: %zd\n", sizeof(FlashLayout), RLM3_FLASH_SIZE - sizeof(FlashLayout));
	ASSERT(sizeof(FlashLayout) == RLM3_FLASH_SIZE);
}

TEST_CASE(ExternalMemoryLayout_Size)
{
	std::printf("Size: %zd Diff: %zd\n", sizeof(ExternalMemoryLayout), RLM3_EXTERNAL_MEMORY_SIZE - sizeof(ExternalMemoryLayout));
	ASSERT(sizeof(ExternalMemoryLayout) == RLM3_EXTERNAL_MEMORY_SIZE);
}
