#include "Test.hpp"
#include "rlm3-sim.hpp"


TEST_CASE(SIM_AddInterrupt_HappyCase)
{
	bool called = false;

	ASSERT(!SIM_HasNextInterrupt());
	SIM_AddInterrupt([&]() { called = true; });
	ASSERT(SIM_HasNextInterrupt());
	ASSERT(!called);
	SIM_RunNextInterrupt();
	ASSERT(called);
	ASSERT(!SIM_HasNextInterrupt());
}

TEST_CASE(SIM_RLM3_Is_IRQ_FromMainThread)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
}

TEST_CASE(SIM_RLM3_Is_IRQ_FromInterrupt)
{
	SIM_AddInterrupt([]() {
		ASSERT(SIM_RLM3_Is_IRQ());
	});
	SIM_RunNextInterrupt();
}

TEST_CASE(SIM_GetNextInterruptTime_HappyCase)
{
	SIM_AddInterrupt([&]() {});
	SIM_AddDelay(10);
	SIM_AddInterrupt([&]() {});

	ASSERT(SIM_GetNextInterruptTime() == 0);
	SIM_RunNextInterrupt();
	ASSERT(SIM_GetNextInterruptTime() == 10);
	SIM_RunNextInterrupt();
}

TEST_CASE(SafeString_HappyCase)
{
	std::string test = SIM_SafeString("\\\r\n\x1F ABCabc123~\x7f\xAB");

	ASSERT(test == "\\\\\\r\\n\\x1F ABCabc123~\\x7F\\xAB");
}

