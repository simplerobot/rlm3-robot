#include "rlm3-memory.h"
#include "rlm3-task.h"
#include "fmc.h"
#include "main.h"
#include "logger.h"
#include "Assert.h"
#include "FreeRTOS.h"
#include <string.h>


LOGGER_ZONE(MEM);


static bool g_is_initialized = false;


extern void RLM3_Mem_Init()
{
	ASSERT(!g_is_initialized);
	if (!g_is_initialized)
	{
		MX_FMC_Init();
		g_is_initialized = true;
		RLM3_Task_Delay(1000); // TODO: Is this actually needed?
	}
}

extern void RLM3_Mem_Deinit()
{
	ASSERT(g_is_initialized);
	if (g_is_initialized)
	{
		HAL_SDRAM_DeInit(&hsdram2);
		g_is_initialized = false;
	}
}

extern bool RLM3_Mem_IsInit()
{
	return g_is_initialized;
}

extern void HAL_SDRAM_RefreshErrorCallback(SDRAM_HandleTypeDef *hsdram)
{
	LOG_ERROR("Refresh Error");
}

extern void* malloc(size_t size)
{
	return pvPortMalloc(size);
}

extern void free(void* ptr)
{
	return vPortFree(ptr);
}

extern void* calloc(size_t count, size_t size)
{
	void* result = malloc(count * size);
	memset(result, 0, count * size);
	return result;
}
