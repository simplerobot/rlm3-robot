#include "Test.hpp"
#include "rlm3-debug.h"
#include <random>


TEST_CASE(DebugOutput_HappyCase)
{
	SIM_Debug_Expect("abc");

	RLM3_Debug_Output('a');
	RLM3_Debug_Output('b');
	RLM3_Debug_Output('c');
}

TEST_CASE(DebugOutput_None)
{
	SIM_Debug_Expect("");
}

TEST_CASE(DebugOutput_NonMatching)
{
	auto test = [] {
		SIM_Debug_Expect("abc");

		RLM3_Debug_Output('a');
		RLM3_Debug_Output('d');
		RLM3_Debug_Output('c');
	};

	TestCaseListItem test_case(test, "test", __FILE__, __LINE__);
	ASSERT(!test_case.Run());
}

TEST_CASE(DebugOutput_Missing)
{
	auto test = [] {
		SIM_Debug_Expect("abc");

		RLM3_Debug_Output('a');
		RLM3_Debug_Output('b');
	};

	TestCaseListItem test_case(test, "test", __FILE__, __LINE__);
	ASSERT(!test_case.Run());
}

TEST_CASE(DebugOutput_Extra)
{
	auto test = [] {
		SIM_Debug_Expect("abc");

		RLM3_Debug_Output('a');
		RLM3_Debug_Output('b');
		RLM3_Debug_Output('c');
		RLM3_Debug_Output('d');
	};

	TestCaseListItem test_case(test, "test", __FILE__, __LINE__);
	ASSERT(!test_case.Run());
}




