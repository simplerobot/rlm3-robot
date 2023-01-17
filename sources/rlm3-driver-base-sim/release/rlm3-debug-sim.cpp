#include "rlm3-debug.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"
#include <cstdio>
#include <queue>


static bool g_monitor_debug_output;
static std::queue<uint8_t> g_expected_debug_output;


extern bool RLM3_Debug_IsActive()
{
	return true;
}

extern bool RLM3_Debug_IsActiveISR()
{
	return true;
}

static bool DebugOutput(uint8_t c)
{
	std::putchar(c);

	if (!g_monitor_debug_output)
		return true;
	if (g_expected_debug_output.empty())
		FAIL("Did not expect any debug output.  Got '%c' 0x%02x", (c >= ' ' && c <= '~') ? c : '?', c);
	char a = g_expected_debug_output.front();
	g_expected_debug_output.pop();
	if (a != c)
		FAIL("Debug output expected '%c' 0x%02x.  Got '%c' 0x%02x", (a >= ' ' && a <= '~') ? a : '?', a, (c >= ' ' && c <= '~') ? c : '?', c);

	return true;
}

extern void RLM3_Debug_Output(uint8_t c)
{
	ASSERT(!SIM_IsISR());
	DebugOutput(c);
}

extern bool RLM3_Debug_OutputISR(uint8_t c)
{
	ASSERT(SIM_IsISR());
	return DebugOutput(c);
}

extern void SIM_Debug_Expect(const char* str)
{
	g_monitor_debug_output = true;
	for (size_t i = 0; str[i] != 0; i++)
		g_expected_debug_output.push(str[i]);
}

TEST_SETUP(SIM_BASE)
{
	g_monitor_debug_output = false;
	while (!g_expected_debug_output.empty())
		g_expected_debug_output.pop();
}

TEST_FINISH(SIM_FINISH)
{
	if (g_monitor_debug_output && !g_expected_debug_output.empty())
	{
		char a = g_expected_debug_output.front();
		FAIL("Debug output missed expected '%c' 0x%02x.", (a >= ' ' && a <= '~') ? a : '?', a);
	}
}

TEST_TEARDOWN(SIM_TEARDOWN)
{
	g_monitor_debug_output = false;
}
