#include "Test.h"
#include "rlm-string.h"
#include "logger.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"


typedef struct TestContext
{
	const TestCaseInfo* test;
	TaskHandle_t child_thread;
	TaskHandle_t parent_thread;
	volatile bool done;
	volatile bool result;
} TestContext;


static TestCaseInfo* g_test_case_head = NULL;
static TestCaseInfo** g_test_case_tail = &g_test_case_head;

static TestHelperInfo* g_test_helper_head = NULL;
static TestHelperInfo** g_test_helper_tail = &g_test_helper_head;

static TestContext* g_test_context = NULL;


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
	ITM_SendChar(c);
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

static __attribute__ ((noreturn)) void ThreadTestCaseFinish(bool result)
{
	g_test_context->result = result;
	g_test_context->done = true;
	xTaskNotifyGive(g_test_context->parent_thread);
	vTaskDelete(g_test_context->child_thread);
	vTaskDelete(NULL);
	while (true)
		;
}

static __attribute__ ((noreturn)) void ThreadTestCaseRun(void* param)
{
	RunHelpers(TEST_HELPER_START);

	(*g_test_context->test->test)();

	RunHelpers(TEST_HELPER_FINISH);

	ThreadTestCaseFinish(true);
}

extern void NotifyAssertFailed(const char* file, long line, const char* function, const char* message, ...)
{
	if (g_test_context == NULL)
	{
		RLM_FnFormat(TestOutput, NULL, "ASSERT FAILED OUTSIDE TESTS\n");
	}
	else if (g_test_context->child_thread == NULL)
	{
		RLM_FnFormat(TestOutput, NULL, "ASSERT FAILED IN PARENT THREAD\n");
	}
	else if (g_test_context->child_thread != xTaskGetCurrentTaskHandle())
	{
		RLM_FnFormat(TestOutput, NULL, "ASSERT FAILED IN SECONDARY THREAD\n");
	}

	va_list args;
	va_start(args, message);
	RLM_FnFormat(TestOutput, NULL, "%s:%ld:1: assert: \u2018", file, line);
	RLM_FnVFormat(TestOutput, NULL, message, args);
	RLM_FnFormat(TestOutput, NULL, "\u2018 in function: %s\n", function);
	va_end(args);

	ThreadTestCaseFinish(false);
}

extern bool RunTestCase(const TestCaseInfo* info)
{
	// Save the previous test context.
	TestContext* previous_context = g_test_context;

	// Create a new test context.
	TestContext current_context = {};
	current_context.test = info;
	current_context.child_thread = NULL;
	current_context.parent_thread = xTaskGetCurrentTaskHandle();
	current_context.done = false;
	current_context.result = false;
	g_test_context = &current_context;

	RunHelpers(TEST_HELPER_SETUP);

	// Run the test on a separate thread.
	BaseType_t result = xTaskCreate(ThreadTestCaseRun, "test", 256, NULL, 24, &current_context.child_thread);
	if (result == pdPASS)
	{
		// Wait for the test to finish.
		while (!current_context.done)
			ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
		current_context.child_thread = NULL;
	}
	else
		TestFormat("Error: unable to create test thread %d.\n", result);

	RunHelpers(TEST_HELPER_TEARDOWN);

	// Restore the previous test context.
	g_test_context = previous_context;

	// Wait just a moment for the idle task to cleanup the thread we created.
	vTaskDelay(1);

	return current_context.result;
}

extern void RLM3_Main_CB()
{
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
}

