#include "Test.h"
#include "rlm-string.h"
#include "logger.h"
#include <setjmp.h>
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


typedef struct TestContext
{
	jmp_buf jump;
	TaskHandle_t task;
	bool failed;
} TestContext;


static TestCaseInfo* g_test_case_head = NULL;
static TestCaseInfo** g_test_case_tail = &g_test_case_head;

static TestHelperInfo* g_test_helper_head = NULL;
static TestHelperInfo** g_test_helper_tail = &g_test_helper_head;

static TestContext* g_test_context = NULL;

static TIM_HandleTypeDef g_htim7;

static volatile size_t g_buffer_head = 0;
static volatile size_t g_buffer_tail = 0;
static volatile char g_buffer[32];
static volatile bool g_is_output_running = false;


extern void AddTestCaseToList(TestCaseInfo* info)
{
	info->next = NULL;
	*g_test_case_tail = info;
	g_test_case_tail = &info->next;
}

extern void AddTestHelperToList(TestHelperInfo* info)
{
	info->next = NULL;
	*g_test_helper_tail = info;
	g_test_helper_tail = &info->next;
}

static void TestOutput(void* ignore, char c)
{
	if (__get_IPSR() != 0U)
	{
		// Just add this character to the buffer if there is room.
		size_t head = g_buffer_head;
		size_t tail = g_buffer_tail;
		if (head - tail < sizeof(g_buffer))
		{
			g_buffer[head % sizeof(g_buffer)] = c;
			g_buffer_head = head + 1;
		}
	}
	else if (g_is_output_running)
	{
		// Wait for the timer to send everything currently in the buffer.
		size_t head = g_buffer_head;
		size_t tail = g_buffer_tail;
		while (head - tail > 0)
			tail = g_buffer_tail;
		// Add this character to the output buffer.
		g_buffer[head % sizeof(g_buffer)] = c;
		g_buffer_head = head + 1;
	}
	else
	{
		ITM_SendChar(c);
	}
}

static void TestFormat(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	RLM_FnVFormat(TestOutput, NULL, format, args);
	va_end(args);
}

extern void logger_format_message(LoggerLevel level, const char* zone, const char* format, ...)
{
	TickType_t tick_count = (__get_IPSR() != 0U) ? xTaskGetTickCountFromISR() : xTaskGetTickCount();

	// Write out this message.
	va_list args;
	va_start(args, format);
	RLM_FnFormat(TestOutput, NULL, "%u %s %s ", (int)tick_count, ToString(level), zone);
	RLM_FnVFormat(TestOutput, NULL, format, args);
	RLM_FnFormat(TestOutput, NULL, "\n");
	va_end(args);
}

static void RunHelpers(TestHelperType type)
{
	for (const TestHelperInfo* cursor = g_test_helper_head; cursor != NULL; cursor = cursor->next)
	{
		if (cursor->type == type)
			(*cursor->fn)();
	}
}

extern void NotifyAssertFailed(const char* file, long line, const char* function, const char* message, ...)
{
	const char* error = "assert failed";

	if (g_test_context == NULL)
	{
		error = "assert failed outside tests";
	}
	else if (__get_IPSR() != 0U)
	{
		error = "assert failed in interrupt";
	}
	else if (g_test_context->task != xTaskGetCurrentTaskHandle())
	{
		error = "assert failed in secondary task";
	}

	va_list args;
	va_start(args, message);
	RLM_FnFormat(TestOutput, NULL, "%s:%ld:1: %s: \u2018", file, line, error);
	RLM_FnVFormat(TestOutput, NULL, message, args);
	RLM_FnFormat(TestOutput, NULL, "\u2018 in function: %s\n", function);
	va_end(args);

	if (g_test_context != NULL)
		g_test_context->failed = true;

	if (g_test_context != NULL && __get_IPSR() == 0U && g_test_context->task == xTaskGetCurrentTaskHandle())
		longjmp(g_test_context->jump, 1);
}

extern bool RunTestCase(const TestCaseInfo* info)
{
	// Save the old context.
	TestContext* previous_context = g_test_context;

	// Setup a new context.
	TestContext current_context;
	current_context.failed = false;
	current_context.task = xTaskGetCurrentTaskHandle();
	g_test_context = &current_context;
	if (setjmp(current_context.jump) != 0)
	{
		RunHelpers(TEST_HELPER_TEARDOWN);
		g_test_context = previous_context;
		return false;
	}

	RunHelpers(TEST_HELPER_SETUP);
	RunHelpers(TEST_HELPER_START);

	(*info->test)();

	RunHelpers(TEST_HELPER_FINISH);
	RunHelpers(TEST_HELPER_TEARDOWN);

	g_test_context = previous_context;
	return !current_context.failed;
}

static void BackgroundTimerInit()
{
	__HAL_RCC_TIM7_CLK_ENABLE();

	g_htim7.Instance = TIM7;
	g_htim7.Init.Prescaler = 0;
	g_htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
	g_htim7.Init.Period = 900;
	g_htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&g_htim7) != HAL_OK)
		Error_Handler();

	TIM_MasterConfigTypeDef master_config = {0};
	master_config.MasterOutputTrigger = TIM_TRGO_RESET;
	master_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&g_htim7, &master_config) != HAL_OK)
		Error_Handler();

	HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(TIM7_IRQn);
	if (HAL_TIM_Base_Start_IT(&g_htim7) != HAL_OK)
		Error_Handler();

	g_is_output_running = true;
}

static void BackgroundTimerDeInit()
{
	g_is_output_running = false;

	if (HAL_TIM_Base_DeInit(&g_htim7) != HAL_OK)
		Error_Handler();

	__HAL_RCC_TIM7_CLK_DISABLE();
}

static void BackgroundTimerFlush()
{
	while (g_buffer_head != g_buffer_tail)
		;
}

extern void TIM7_IRQHandler()
{
	__HAL_TIM_CLEAR_IT(&g_htim7, TIM_IT_UPDATE);
	size_t head = g_buffer_head;
	size_t tail = g_buffer_tail;
	if (tail < head && ITM->PORT[0U].u32 != 0)
	{
		ITM->PORT[0U].u8 = g_buffer[tail % sizeof(g_buffer)];
		g_buffer_tail = tail + 1;
	}
}

extern void RLM3_Main_CB()
{
	BackgroundTimerInit();
	TestFormat("== RUNNING TEST CASES ==\n");

	size_t total_test_count = 0;
	size_t passed_test_count = 0;

	for (const TestCaseInfo* cursor = g_test_case_head; cursor != NULL; cursor = cursor->next)
	{
		TestFormat("=== TEST: %s ===\n", cursor->name);

		total_test_count++;
		if (RunTestCase(cursor))
		{
			passed_test_count++;
		}
		else
		{
			TestFormat("=== TEST FAILED: %s File '%s' line %d ===\n", cursor->name, cursor->file, cursor->line);
		}

		BackgroundTimerFlush();
	}

	TestFormat("== TEST SUMMARY ==\n");
	TestFormat("%d Total Test\n", total_test_count);
	TestFormat("%d Tests Passed\n", passed_test_count);
	if (total_test_count == passed_test_count)
	{
		TestFormat("== TESTS PASSED ==\n");
		TestFormat("EOT PASS\n");
	}
	else
	{
		TestFormat("%d Failed Tests\n", total_test_count - passed_test_count);
		TestFormat("== TESTS FAILED ==\n");
		TestFormat("EOT FAIL\n");
	}

	BackgroundTimerFlush();
	BackgroundTimerDeInit();
}

