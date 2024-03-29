#include "Test.hpp"
#include <stdexcept>
#include <thread>


TEST_CASE(TestCaseListItem_Constructor_HappyCase)
{
	TestCaseListItem test([](){}, "TEST_CONSTRUCTOR", "FILE", 1234);
}

TEST_CASE(TestCaseListItem_Run_HappyCase)
{
	TestCaseListItem test([](){}, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(test.Run());
}

TEST_CASE(TestCaseListItem_Run_TestFails)
{
	TestCaseListItem test([](){ ASSERT(false); }, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test.Run());
}

TEST_CASE(ASSERT_Passes)
{
	auto test = []() { ASSERT(true); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(test_case.Run());
}

TEST_CASE(ASSERT_Fails)
{
	auto test = []() { ASSERT(false); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(ASSERT_THROWS_Passes)
{
	auto test = []() { ASSERT_THROWS(throw std::runtime_error("error")); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(test_case.Run());
}

TEST_CASE(ASSERT_THROWS_Fails)
{
	auto test = []() { ASSERT_THROWS((void)0); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(ASSERT_THROWS_Asserts)
{
	auto test = []() { ASSERT_THROWS(ASSERT(false)); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(ASSERT_ASSERTS_Passes)
{
	auto test = []() { ASSERT_ASSERTS(ASSERT(false)); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(test_case.Run());
}

TEST_CASE(ASSERT_ASSERTS_Fails)
{
	auto test = []() { ASSERT_ASSERTS(ASSERT(true)); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(ASSERT_ASSERTS_Throws)
{
	auto test = []() { ASSERT_ASSERTS(throw std::runtime_error("error")); };
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}


TEST_CASE(ASSERT_Fails_SecondaryThread)
{
	auto test = []() {
		std::thread secondary_thread([]() { ASSERT(false); });
		secondary_thread.join();
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
}

TEST_CASE(ASSERT_Fails_Destructor)
{
	class DestructorFailure
	{
	public:
		~DestructorFailure() { ASSERT(false); }
	};

	auto test = []() {
		DestructorFailure test;
	};
	TestCaseListItem test_case(test, "TEST_CONSTRUCTOR", "FILE", 1234);

	ASSERT(!test_case.Run());
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

TEST_CASE(TEST_HELPER_Counts)
{
	auto test = []() { ASSERT(g_active_count == 2); };
	TestCaseListItem test_case(test, "NESTED_TEST", "FILE", 1234);

	ASSERT(g_active_count == 1);
	ASSERT(test_case.Run());
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
	TestCaseListItem test_case([]{}, "ALWAYS_PASSES", "FILE", 1234);

	g_should_start_fn_fail = true;
	ASSERT(!test_case.Run());

	ASSERT(test_case.Run());

	g_should_finish_fn_fail = true;
	ASSERT(!test_case.Run());
}


