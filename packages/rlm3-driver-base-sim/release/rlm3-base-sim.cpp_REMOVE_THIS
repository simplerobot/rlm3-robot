#include "rlm3-base.h"
#include "rlm3-sim.hpp"
#include <cstdio>
#include "Test.hpp"
#include <cstring>
#include <sstream>
#include <queue>


static const uint8_t g_default_device_id[12] = { 0x39, 0x00, 0x3d, 0x00, 0x11, 0x51, 0x36, 0x30, 0x34, 0x38, 0x37, 0x30 };
static uint8_t g_device_id[12];
static uint32_t g_device_short_id = 0;
static bool g_has_short_id = false;

static bool g_monitor_debug_output;
static std::queue<uint8_t> g_expected_debug_output;


extern bool RLM3_IsIRQ()
{
	return SIM_RLM3_Is_IRQ();
}

extern bool RLM3_IsSchedulerRunning()
{
	return true;
}

extern void SIM_ExpectDebugOutput(const char* str)
{
	g_monitor_debug_output = true;
	for (size_t i = 0; str[i] != 0; i++)
		g_expected_debug_output.push(str[i]);
}

static void GotDebugOutput(uint8_t c)
{
	if (!g_monitor_debug_output)
		return;
	if (g_expected_debug_output.empty())
		FAIL("Did not expect any debug output.  Got '%c' 0x%02x", (c >= ' ' && c <= '~') ? c : '?', c);
	char a = g_expected_debug_output.front();
	g_expected_debug_output.pop();
	if (a != c)
		FAIL("Debug output expected '%c' 0x%02x.  Got '%c' 0x%02x", (a >= ' ' && a <= '~') ? a : '?', a, (c >= ' ' && c <= '~') ? c : '?', c);
}

extern bool RLM3_IsDebugOutput()
{
	return true;
}

extern void RLM3_DebugOutput(uint8_t c)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	GotDebugOutput(c);
	std::putchar(c);
}

extern bool RLM3_DebugOutputFromISR(uint8_t c)
{
	ASSERT(SIM_RLM3_Is_IRQ());
	GotDebugOutput(c);
	std::putchar(c);
	return true;
}

extern void RLM3_GetUniqueDeviceId(uint8_t id_out[12])
{
	::memcpy(id_out, g_device_id, sizeof(g_device_id));
}

extern uint32_t RLM3_GetUniqueDeviceShortId()
{
	if (g_has_short_id)
		return g_device_short_id;
	uint8_t id[12];
	RLM3_GetUniqueDeviceId(id);
	uint32_t hash = 0;
	for (uint8_t x : id)
		hash = hash * 65599 + x;
	return hash;
}

extern void SIM_SetUniqueDeviceId(const uint8_t id[12])
{
	::memcpy(g_device_id, id, sizeof(g_device_id));
}

extern void SIM_SetUniqueDeviceShortId(uint32_t id)
{
	g_device_short_id = id;
	g_has_short_id = true;
}

extern std::string SIM_SafeString(const std::string& input)
{
	std::ostringstream out;
	for (char c : input)
	{
		if (c == '\\')
		{
			out.put('\\');
			out.put('\\');
		}
		else if (' ' <= c && c <= '~')
		{
			out.put(c);
		}
		else if (c == '\r')
		{
			out.put('\\');
			out.put('r');
		}
		else if (c == '\n')
		{
			out.put('\\');
			out.put('n');
		}
		else
		{
			out.put('\\');
			out.put('x');
			static const char* k_hex_digits = "0123456789ABCDEF";
			out.put(k_hex_digits[(c >> 4) & 0x0F]);
			out.put(k_hex_digits[(c >> 0) & 0x0F]);
		}
	}
	return out.str();
}

TEST_SETUP(SIM_BASE)
{
	::memcpy(g_device_id, g_default_device_id, sizeof(g_device_id));
	g_has_short_id = false;
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
