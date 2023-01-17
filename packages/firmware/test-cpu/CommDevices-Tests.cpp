#include "Test.hpp"
#include "rlm3-wifi.h"
#include "rlm3-sim.hpp"
#include <queue>
#include "../../firmware/main/CommDevices.hpp"


static void MOCK_CommInput_Message(uint8_t byte);
static void MOCK_CommInput_Failure(uint8_t byte);
static void MOCK_CommInput_Reset();

static void MOCK_CommOutput_OpenConnection(size_t link_id);
static void MOCK_CommOutput_CloseConnection(size_t link_id);


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
	MOCK_CommOutput_OpenConnection(3);
	SIM_WIFI_ReceiveByte(3, 0x12);
	MOCK_CommInput_Message(0x12);
	SIM_WIFI_Disconnect(3);
	MOCK_CommInput_Reset();
	bool done = false;
	SIM_AddInterrupt([&] { done = true; SIM_Give(); });

	ASSERT(CommDevices_Init());
	while (!done)
		RLM3_Task_Take();
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
	MOCK_CommOutput_OpenConnection(3);
	SIM_WIFI_ReceiveByte(3, 0x12);
	MOCK_CommInput_Message(0x12);
	SIM_WIFI_Connect(4);
	MOCK_CommOutput_CloseConnection(3);
	MOCK_CommInput_Reset();
	MOCK_CommOutput_OpenConnection(4);
	SIM_WIFI_ReceiveByte(3, 0x13);
	MOCK_CommOutput_CloseConnection(3);
	SIM_WIFI_ReceiveByte(4, 0x14);
	MOCK_CommInput_Message(0x14);
	SIM_WIFI_Disconnect(3);
	bool done = false;
	SIM_AddInterrupt([&] { done = true; SIM_Give(); });

	ASSERT(CommDevices_Init());
	while (!done)
		RLM3_Task_Take();
	CommDevices_Deinit();
}

TEST_CASE(CommDevices_FailedMessage)
{
	SIM_WIFI_SetLocalNetwork("RLM3", "ABCD1234", 2, "10.168.154.1", "37649");
	SIM_WIFI_Connect(3);
	MOCK_CommInput_Reset();
	MOCK_CommOutput_OpenConnection(3);
	SIM_WIFI_ReceiveByte(3, 0x12);
	MOCK_CommInput_Failure(0x12);
	MOCK_CommInput_Reset();
	MOCK_CommOutput_CloseConnection(3);
	SIM_WIFI_ReceiveByte(3, 0x13);
	MOCK_CommOutput_CloseConnection(3);
	SIM_WIFI_Disconnect(3);
	bool done = false;
	SIM_AddInterrupt([&] { done = true; SIM_Give(); });

	ASSERT(CommDevices_Init());
	while (!done)
		RLM3_Task_Take();
	CommDevices_Deinit();
}


// Simple mocking code for the CommInput methods.

static std::queue<std::string> g_mock_method;

static std::queue<uint8_t> g_mock_comm_input_messages;
static std::queue<bool> g_mock_comm_input_returns;
static std::queue<size_t> g_mock_comm_output_links;

TEST_SETUP(SIM_CommInput_Init)
{
	while (!g_mock_method.empty())
		g_mock_method.pop();
	while (!g_mock_comm_input_messages.empty())
		g_mock_comm_input_messages.pop();
	while (!g_mock_comm_input_returns.empty())
		g_mock_comm_input_returns.pop();
	while (!g_mock_comm_output_links.empty())
		g_mock_comm_output_links.pop();
}

TEST_FINISH(SIM_CommInput_Finish)
{
	ASSERT(g_mock_method.empty());
	ASSERT(g_mock_comm_input_messages.empty());
	ASSERT(g_mock_comm_input_returns.empty());
	ASSERT(g_mock_comm_output_links.empty());
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

static void MOCK_CommOutput_OpenConnection(size_t link_id)
{
	g_mock_method.push("Open");
	g_mock_comm_output_links.push(link_id);
}

static void MOCK_CommOutput_CloseConnection(size_t link_id)
{
	g_mock_method.push("Close");
	g_mock_comm_output_links.push(link_id);
}

extern bool CommInput_PutMessageByteISR(uint8_t byte)
{
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

extern void CommInput_ResetPipeISR()
{
	ASSERT(!g_mock_method.empty());
	ASSERT(g_mock_method.front() == "Reset");
	g_mock_method.pop();
}

extern void CommOutput_OpenConnectionISR(size_t link_id)
{
	ASSERT(!g_mock_method.empty());
	ASSERT(g_mock_method.front() == "Open");
	g_mock_method.pop();

	ASSERT(!g_mock_comm_output_links.empty());
	size_t expected = g_mock_comm_output_links.front();
	g_mock_comm_output_links.pop();

	ASSERT(link_id == expected);
}

extern void CommOutput_CloseConnectionISR(size_t link_id)
{
	ASSERT(!g_mock_method.empty());
	ASSERT(g_mock_method.front() == "Close");
	g_mock_method.pop();

	ASSERT(!g_mock_comm_output_links.empty());
	size_t expected = g_mock_comm_output_links.front();
	g_mock_comm_output_links.pop();

	ASSERT(link_id == expected);
}

TEST_SETUP(CommDevices_Setup)
{
	SIM_WIFI_IgnoreGpio();
}
