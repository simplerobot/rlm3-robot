#include "Test.h"
#include "FreeRTOS.h"
#include "task.h"


static void EmptyTestFn()
{
}

static void PassTestFn()
{
	ASSERT(true);
}

static void FailTestFn()
{
	ASSERT(false);
}

TEST_CASE(Test_Empty)
{
}

TEST_CASE(Test_Pass)
{
	ASSERT(true);
}

TEST_CASE(Test_RunTestCase_Empty)
{
	TestCaseInfo test = {};
	test.test = EmptyTestFn;

	ASSERT(RunTestCase(&test));
}

TEST_CASE(Test_RunTestCase_Pass)
{
	TestCaseInfo test = {};
	test.test = PassTestFn;

	ASSERT(RunTestCase(&test));
}

TEST_CASE(Test_RunTestCase_Fail)
{
	TestCaseInfo test = {};
	test.test = FailTestFn;

	ASSERT(!RunTestCase(&test));
}

static void FailTaskTestFn(void* param)
{
	ASSERT(false);
}

static void FailThreadTestFn()
{
	xTaskCreate(FailTaskTestFn, "test", 256, NULL, 24, NULL);
	while (true)
		;
}

TEST_CASE(Test_RunTestCase_FailInSecondaryThread)
{
	TestCaseInfo test = {};
	test.test = FailThreadTestFn;

	ASSERT(!RunTestCase(&test));
}

static uint32_t g_total_setup_count = 0;
static uint32_t g_total_teardown_count = 0;
static uint32_t g_active_count = 0;

TEST_SETUP(COUNTS)
{
	g_total_setup_count++;
	g_active_count++;
}

TEST_TEARDOWN(COUNTS)
{
	ASSERT(g_active_count >= 0);
	g_total_teardown_count++;
	g_active_count--;
}

static void TestActiveCountFn()
{
	ASSERT(g_active_count == 2);
}

TEST_CASE(TEST_HELPER_Counts)
{
	TestCaseInfo test = {};
	test.test = TestActiveCountFn;

	ASSERT(g_active_count == 1);
	ASSERT(RunTestCase(&test));
	ASSERT(g_active_count == 1);
}

static bool g_should_start_fn_fail = false;
static bool g_should_finish_fn_fail = false;

TEST_START(FAILS)
{
	if (g_should_start_fn_fail)
	{
		g_should_start_fn_fail = false;
		ASSERT(false);
	}
}

TEST_FINISH(FAILS)
{
	if (g_should_finish_fn_fail)
	{
		g_should_finish_fn_fail = false;
		ASSERT(false);
	}
}

TEST_CASE(TEST_HELPER_Fails)
{
	TestCaseInfo test = {};
	test.test = PassTestFn;

	g_should_start_fn_fail = true;
	ASSERT(!RunTestCase(&test));

	ASSERT(RunTestCase(&test));

	g_should_finish_fn_fail = true;
	ASSERT(!RunTestCase(&test));

	ASSERT(RunTestCase(&test));
}


