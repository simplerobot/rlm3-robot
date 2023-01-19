#include "Test.hpp"
#include "rlm3-sim.hpp"


TEST_CASE(SIM_AddInterrupt_HappyCase)
{
	bool called = false;

	ASSERT(!SIM_HasNextHandler());
	SIM_AddInterrupt([&]() { called = true; });
	ASSERT(SIM_HasNextHandler());
	ASSERT(!called);
	SIM_RunNextHandler();
	ASSERT(called);
	ASSERT(!SIM_HasNextHandler());
}

TEST_CASE(SIM_RLM3_Is_IRQ_FromMainThread)
{
	ASSERT(!SIM_IsISR());
}

TEST_CASE(SIM_RLM3_Is_IRQ_FromInterrupt)
{
	SIM_AddInterrupt([]() {
		ASSERT(SIM_IsISR());
	});
	SIM_RunNextHandler();
}

TEST_CASE(SIM_GetNextInterruptTime_HappyCase)
{
	SIM_AddInterrupt([&]() {});
	SIM_AddDelay(10);
	SIM_AddInterrupt([&]() {});

	ASSERT(SIM_GetNextHandlerTime() == 0);
	SIM_RunNextHandler();
	ASSERT(SIM_GetNextHandlerTime() == 10);
	SIM_RunNextHandler();
}

TEST_CASE(SafeString_HappyCase)
{
	std::string test = SIM_SafeString("\\\r\n\x1F ABCabc123~\x7f\xAB");

	ASSERT(test == "\\\\\\r\\n\\x1F ABCabc123~\\x7F\\xAB");
}

