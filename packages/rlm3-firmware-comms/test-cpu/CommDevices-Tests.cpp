#include "Test.hpp"
#include "CommDevices.hpp"
#include "rlm3-wifi.h"
#include "rlm3-sim.hpp"
#include <queue>


static void MOCK_CommInput_Message(uint8_t byte);
static void MOCK_CommInput_Failure(uint8_t byte);
static void MOCK_CommInput_Reset();


TEST_CASE(CommDevices_Lifecycle)
{
	SIM_WIFI_SetLocalNetwork("RLM3", "ABCD1234", 2, "10.168.154.1", "37649");

	ASSERT(CommDevices_Init());
	CommDevices_Deinit();
}

TEST_CASE(CommDevices_HappyCase)
{
	SIM_WIFI_SetLocalNetwork("RLM3", "ABCD1234", 2, "10.168.154.1", "37649");
	SIM_WIFI_Connect(3);
	MOCK_CommInput_Reset();
	SIM_WIFI_ReceiveByte(3, 0x12);
	MOCK_CommInput_Message(0x12);
	SIM_WIFI_Disconnect(3);
	MOCK_CommInput_Reset();
	bool done = false;
	SIM_AddInterrupt([&] { done = true; SIM_Give(); });

	ASSERT(CommDevices_Init());
	while (!done)
		RLM3_Take();
	CommDevices_Deinit();
}

TEST_CASE(CommDevices_InitFailed)
{
	SIM_WIFI_InitFailure();

	ASSERT(!CommDevices_Init());
}

TEST_CASE(CommDevices_LocalNetworkFailed)
{
	ASSERT(!CommDevices_Init());
}

TEST_CASE(CommDevices_SecondConnection)
{
	SIM_WIFI_SetLocalNetwork("RLM3", "ABCD1234", 2, "10.168.154.1", "37649");
	SIM_WIFI_Connect(3);
	MOCK_CommInput_Reset();
	SIM_WIFI_ReceiveByte(3, 0x12);
	MOCK_CommInput_Message(0x12);
	SIM_WIFI_Connect(4);
	MOCK_CommInput_Reset();
	SIM_WIFI_ReceiveByte(3, 0x13);
	SIM_WIFI_ReceiveByte(4, 0x14);
	MOCK_CommInput_Message(0x14);
	SIM_WIFI_Disconnect(3);
	bool done = false;
	SIM_AddInterrupt([&] { done = true; SIM_Give(); });

	ASSERT(CommDevices_Init());
	while (!done)
		RLM3_Take();
	CommDevices_Deinit();
}

TEST_CASE(CommDevices_FailedMessage)
{
	SIM_WIFI_SetLocalNetwork("RLM3", "ABCD1234", 2, "10.168.154.1", "37649");
	SIM_WIFI_Connect(3);
	MOCK_CommInput_Reset();
	SIM_WIFI_ReceiveByte(3, 0x12);
	MOCK_CommInput_Failure(0x12);
	MOCK_CommInput_Reset();
	SIM_WIFI_ReceiveByte(3, 0x13);
	SIM_WIFI_Disconnect(3);
	bool done = false;
	SIM_AddInterrupt([&] { done = true; SIM_Give(); });

	ASSERT(CommDevices_Init());
	while (!done)
		RLM3_Take();
	CommDevices_Deinit();
}


// Simple mocking code for the CommInput methods.

static std::queue<std::string> g_mock_method;

static std::queue<uint8_t> g_mock_comm_input_messages;
static std::queue<bool> g_mock_comm_input_returns;

TEST_SETUP(SIM_CommInput_Init)
{
	while (!g_mock_method.empty())
		g_mock_method.pop();
	while (!g_mock_comm_input_messages.empty())
		g_mock_comm_input_messages.pop();
	while (!g_mock_comm_input_returns.empty())
		g_mock_comm_input_returns.pop();
}

TEST_FINISH(SIM_CommInput_Finish)
{
	ASSERT(g_mock_method.empty());
	ASSERT(g_mock_comm_input_messages.empty());
	ASSERT(g_mock_comm_input_returns.empty());
}

static void MOCK_CommInput_Message(uint8_t byte)
{
	g_mock_method.push("Message");
	g_mock_comm_input_messages.push(byte);
	g_mock_comm_input_returns.push(true);
}

static void MOCK_CommInput_Failure(uint8_t byte)
{
	g_mock_method.push("Message");
	g_mock_comm_input_messages.push(byte);
	g_mock_comm_input_returns.push(false);
}

static void MOCK_CommInput_Reset()
{
	g_mock_method.push("Reset");
}

extern bool CommInput_PutMessageByteISR_MOCK(uint8_t byte)
{
std::printf("Calling CommInput_PutMessageByteISR_MOCK(0x%02x)\n", byte);
	ASSERT(!g_mock_method.empty());
	ASSERT(g_mock_method.front() == "Message");
	g_mock_method.pop();

	ASSERT(!g_mock_comm_input_messages.empty());
	uint8_t expected = g_mock_comm_input_messages.front();
	g_mock_comm_input_messages.pop();

	ASSERT(!g_mock_comm_input_returns.empty());
	bool success = g_mock_comm_input_returns.front();
	g_mock_comm_input_returns.pop();

	ASSERT(byte == expected);
	return success;
}

extern void CommInput_ResetPipeISR_MOCK()
{
	std::printf("Calling CommInput_ResetPipeISR_MOCK()\n");
	ASSERT(!g_mock_method.empty());
	ASSERT(g_mock_method.front() == "Reset");
	g_mock_method.pop();
}


