#include "rlm3-base.h"
#include "rlm3-task.h"
#include "rlm3-gpio.h"
#include "rlm3-debug.h"
#include "rlm3-memory.h"
#include "rlm3-flash.h"
#include "rlm3-lock.h"
#include "rlm3-timer.h"
#include "rlm-string.h"
#include "logger.h"
#include "CommDevices.hpp"
#include "CommInput.hpp"
#include "CommOutput.hpp"
#include "Control.hpp"
#include "Settings.h"
#include <cstdarg>
#include <cstring>


extern "C" void Error_Handler(void);


LOGGER_ZONE(MAIN);


static bool g_logs_enabled = false;
static volatile uint8_t g_log_task_count = 0;
static volatile uint32_t g_log_front;


static void Main_CommInput_Run()
{
	CommInput_RunTask();
}

static void Main_CommOutput_Run()
{
	CommOutput_RunTask();
}

static void Main_Control_Run()
{
	Control_RunTask();
}

static void InitializeLogs()
{
	ExternalMemoryLayout* external_mem = (ExternalMemoryLayout*)RLM3_EXTERNAL_MEMORY_DATA;

	// If the log data in the external memory is missing, reset the buffer.
	if (external_mem->log_magic != SETTINGS_EXTERN_LOG_MAGIC)
	{
		external_mem->log_head = 0;
		external_mem->log_tail = 0;
		external_mem->log_magic = SETTINGS_EXTERN_LOG_MAGIC;
	}

	// If the log data in the external memory is invalid, reset the buffer.
	if (external_mem->log_head - external_mem->log_tail > sizeof(external_mem->log_buffer))
	{
		external_mem->log_head = 0;
		external_mem->log_tail = 0;
	}

	// TODO: If the buffer is more than half full, drop the oldest half to make room for new logs.
	if (external_mem->log_head - external_mem->log_tail > sizeof(external_mem->log_buffer) / 2)
	{
		// TODO:
	}

	g_log_front = external_mem->log_head;
	g_logs_enabled = true;

	// Start a timer to dump logs to the console.
	RLM3_Timer2_Init(1000);
}

static __attribute__((unused)) void InitializeConfiguration() // @suppress("Unused static function")
{
	ExternalMemoryLayout* external_mem = (ExternalMemoryLayout*)RLM3_EXTERNAL_MEMORY_DATA;

	// Load settings from flash into memory.
	if (!RLM3_Flash_Read(0x0000, (uint8_t*)&external_mem->configuration, sizeof(external_mem->configuration)))
	{
		LOG_FATAL("Unable to read flash");
		Error_Handler();
	}

	if (external_mem->configuration.magic != SETTINGS_FLASH_CONFIG_MAGIC)
	{
		LOG_FATAL("Configuration not initialized");
		Error_Handler();
	}

	if (external_mem->configuration.version > 0)
	{
		LOG_FATAL("Incompatible configuration");
		Error_Handler();
	}

}

extern "C" void RLM3_Main_CB()
{
	LOG_ALWAYS("GPIO Init");
	RLM3_GPIO_Init();

	LOG_ALWAYS("Mem Init");
	RLM3_Mem_Init();

	LOG_ALWAYS("Flash Init");
	RLM3_Flash_Init();

	LOG_ALWAYS("Log Init");
	InitializeLogs();

	// TODO: Check for SW update.

	LOG_ALWAYS("Config Init");
//	InitializeConfiguration();

	CommDevices_Init();
	CommOutput_Init();

	// TODO: Need to compute stack sizes.
	RLM3_Task_Create(Main_CommInput_Run, 256, "CommInput");
	RLM3_Task_Create(Main_CommOutput_Run, 256, "CommOutput");
	RLM3_Task_Create(Main_Control_Run, 256, "Control");

	// What should this thread do?
	while (true)
	{
		RLM3_Task_Delay(1000);
		RLM3_GPIO_StatusLight_SetHigh();
		RLM3_Task_Delay(1000);
		RLM3_GPIO_StatusLight_SetLow();
	}
}

static void DebugLogFn(void* data, char c)
{
	RLM3_Debug_OutputSafe(c);
}

static void BufferLogFn(void* data, char c)
{
	ExternalMemoryLayout* external_mem = (ExternalMemoryLayout*)RLM3_EXTERNAL_MEMORY_DATA;

	uint32_t& index = *(uint32_t*)data;
	external_mem->log_buffer[(index++) % sizeof(external_mem->log_buffer)] = c;
}

extern "C" void logger_format_message(LoggerLevel level, const char* zone, const char* format, ...)
{
	ExternalMemoryLayout* external_mem = (ExternalMemoryLayout*)RLM3_EXTERNAL_MEMORY_DATA;
	va_list args;

	RLM3_Time time = RLM3_Time_GetSafe();
	const char* level_str = ToString(level);

	// If logs have not been enabled, just synchronously write to the debug log.
	if (!g_logs_enabled)
	{
		va_start(args, format);
		RLM_FnFormat(DebugLogFn, nullptr, "%u %s %s ", (unsigned int)time, level_str, zone);
		RLM_FnVFormat(DebugLogFn, nullptr, format, args);
		RLM_FnFormat(DebugLogFn, nullptr, "\n");
		va_end(args);
		return;
	}

	// Measure our log length.
	size_t length = 0;
	va_start(args, format);
	length += RLM_FormatNoNul(nullptr, 0, "%u %s %s ", (unsigned int)time, level_str, zone);
	length += RLM_VFormatNoNul(nullptr, 0, format, args);
	length += RLM_FormatNoNul(nullptr, 0, "\n");
	va_end(args);

	// Allocate space from the log buffer.
	uint32_t saved_level_a = RLM3_Lock_EnterCriticalSafe();
	size_t cursor = g_log_front;
	if (cursor + length - external_mem->log_tail > sizeof(external_mem->log_buffer))
	{
		// The buffer does not have enough space.
		RLM3_Lock_ExitCriticalSafe(saved_level_a);
		return;
	}
	g_log_front = cursor + length;
	g_log_task_count++;
	RLM3_Lock_ExitCriticalSafe(saved_level_a);

	// Write the log into the buffer.
	va_start(args, format);
	RLM_FnFormat(BufferLogFn, &cursor, "%u %s %s ", (unsigned int)time, level_str, zone);
	RLM_FnVFormat(BufferLogFn, &cursor, format, args);
	RLM_FnFormat(BufferLogFn, &cursor, "\n");
	va_end(args);

	// Unlock and if last task, release all to output task.
	uint32_t saved_level_b = RLM3_Lock_EnterCriticalSafe();
	g_log_task_count--;
	if (g_log_task_count == 0)
		external_mem->log_head = g_log_front;
	RLM3_Lock_ExitCriticalSafe(saved_level_b);
}

extern "C" void RLM3_Timer2_Event_CB_ISR()
{
	ExternalMemoryLayout* external_mem = (ExternalMemoryLayout*)RLM3_EXTERNAL_MEMORY_DATA;

	// Check if there is any pending output.
	uint32_t tail = external_mem->log_tail;
	uint32_t head = external_mem->log_head;
	if (tail == head)
		return;

	// Try to write the log data.
	char c = external_mem->log_buffer[tail % sizeof(external_mem->log_buffer)];
	if (!RLM3_Debug_OutputISR(c))
		return;

	// Move on to the next character.
	external_mem->log_tail = tail + 1;
}
