#include "Test.hpp"
#include "rlm3-wifi.h"
#include "rlm3-gpio.h"
#include "rlm3-task.h"
#include "rlm3-uart.h"
#include "rlm3-sim.hpp"
#include <cstring>
#include <vector>
#include "logger.h"

#include "rlm3-base.h"


volatile RLM3_Task g_client_thread = nullptr;
volatile size_t g_recv_buffer_count = 0;
volatile uint8_t g_recv_buffer_data[32];

volatile size_t g_network_callback_count = 0;
std::vector<std::pair<size_t, bool>> g_network_connect_calls;
std::vector<std::pair<size_t, bool>> g_network_disconnect_calls;


extern void RLM3_WIFI_Receive_Callback(size_t link_id, uint8_t data)
{
	ASSERT(link_id == 2);
	if (g_recv_buffer_count < sizeof(g_recv_buffer_data))
		g_recv_buffer_data[g_recv_buffer_count] = data;
	g_recv_buffer_count++;
	RLM3_GiveFromISR(g_client_thread);
}

extern void RLM3_WIFI_NetworkConnect_Callback(size_t link_id, bool local_connection)
{
	g_network_callback_count++;
	g_network_connect_calls.emplace_back(link_id, local_connection);
	RLM3_GiveFromISR(g_client_thread);
}

extern void RLM3_WIFI_NetworkDisconnect_Callback(size_t link_id, bool local_connection)
{
	g_network_callback_count++;
	g_network_disconnect_calls.emplace_back(link_id, local_connection);
	RLM3_GiveFromISR(g_client_thread);
}

TEST_CASE(RLM3_WIFI_IsInit_Uninitialized)
{
	ASSERT(!RLM3_WIFI_IsInit());
}

static void ExpectInit()
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");
	SIM_RLM3_UART4_Receive("ATE0\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMODE=0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMUX=1\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWMODE_CUR=1\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWAUTOCONN=0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
}

TEST_CASE(RLM3_WIFI_Init_HappyCase)
{
	ExpectInit();

	ASSERT(RLM3_WIFI_Init());

	ASSERT(RLM3_WIFI_IsInit());
	ASSERT(SIM_GPIO_IsClockEnabled(GPIOG));
	ASSERT(RLM3_UART4_IsInit());
	ASSERT(SIM_RLM3_UART4_GetBaudrate() == 115200);
	ASSERT(SIM_GPIO_GetMode(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin) == GPIO_MODE_OUTPUT_PP);
	ASSERT(SIM_GPIO_GetMode(WIFI_BOOT_MODE_GPIO_Port, WIFI_BOOT_MODE_Pin) == GPIO_MODE_OUTPUT_PP);
	ASSERT(SIM_GPIO_GetMode(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin) == GPIO_MODE_OUTPUT_PP);
	ASSERT(SIM_GPIO_Read(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin));
	ASSERT(SIM_GPIO_Read(WIFI_BOOT_MODE_GPIO_Port, WIFI_BOOT_MODE_Pin));
	ASSERT(SIM_GPIO_Read(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_Init_PingTimeout)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_PingFailure)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nFAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_EchoTimeout)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_EchoFailure)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");
	SIM_RLM3_UART4_Receive("ATE0\r\nFAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_SetTransmissionModeFailure)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");
	SIM_RLM3_UART4_Receive("ATE0\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMODE=0\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_SetMultipleConnectionsFailure)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");
	SIM_RLM3_UART4_Receive("ATE0\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMODE=0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMUX=1\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_SetAutoconnectFailure)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");
	SIM_RLM3_UART4_Receive("ATE0\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMODE=0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMUX=1\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWMODE_CUR=1\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_ManualConnectFailure)
{
	SIM_RLM3_UART4_Transmit("AT\r\n");
	SIM_RLM3_UART4_Receive("AT\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("ATE0\r\n");
	SIM_RLM3_UART4_Receive("ATE0\r\nOK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMODE=0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPMUX=1\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWMODE_CUR=1\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWAUTOCONN=0\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_DeInit_HappyCase)
{
	ExpectInit();

	RLM3_WIFI_Init();
	RLM3_WIFI_Deinit();
	ASSERT(!RLM3_WIFI_IsInit());
	ASSERT(SIM_GPIO_IsClockEnabled(GPIOG));
	ASSERT(!RLM3_UART4_IsInit());
	ASSERT(!SIM_GPIO_IsEnabled(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin));
	ASSERT(!SIM_GPIO_IsEnabled(WIFI_BOOT_MODE_GPIO_Port, WIFI_BOOT_MODE_Pin));
	ASSERT(!SIM_GPIO_IsEnabled(WIFI_RESET_GPIO_Port, WIFI_RESET_Pin));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_GetVersion_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+GMR\r\n");
	SIM_RLM3_UART4_Receive("AT version:255.254.253.252-dev(blah)\r\nSDK version:v251.250.249.248-ge7acblah\r\ncompile time(xxxx)\r\nBin version:2.1.0(Mini)\r\n\r\nOK\r\n");

	RLM3_WIFI_Init();

	uint32_t at_version = 0;
	uint32_t sdk_version = 0;
	ASSERT(RLM3_WIFI_GetVersion(&at_version, &sdk_version));

	ASSERT(at_version == 0xFFFEFDFC);
	ASSERT(sdk_version == 0xFBFAF9F8);
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_GetVersion_Timeout)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+GMR\r\n");

	RLM3_WIFI_Init();
	uint32_t at_version = 0;
	uint32_t sdk_version = 0;
	ASSERT(!RLM3_WIFI_GetVersion(&at_version, &sdk_version));
}

TEST_CASE(RLM3_WIFI_GetVersion_Failure)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+GMR\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	uint32_t at_version = 0;
	uint32_t sdk_version = 0;
	ASSERT(!RLM3_WIFI_GetVersion(&at_version, &sdk_version));
}

TEST_CASE(RLM3_WIFI_NetworkConnect_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_AddDelay(10);
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_AddDelay(10);
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_AddDelay(10);
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	ASSERT(RLM3_WIFI_NetworkConnect("test-sid", "test-pwd"));
	ASSERT(RLM3_WIFI_IsNetworkConnected());
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_NetworkConnect_Error)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_AddDelay(10);
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	ASSERT(!RLM3_WIFI_NetworkConnect("test-sid", "test-pwd"));
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_NetworkDisconnect_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWQAP\r\n");
	SIM_RLM3_UART4_Receive("WIFI DISCONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_NetworkDisconnect_Failure)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWQAP\r\n");
	SIM_RLM3_UART4_Receive("WIFI DISCONNECT\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_NetworkDisconnect_NotConnected)
{
	ExpectInit();

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_ServerConnect_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	ASSERT(RLM3_WIFI_ServerConnect(2, "test-server", "test-port"));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 1);
	ASSERT(g_network_connect_calls.size() == 1);
	ASSERT(g_network_connect_calls.front() == std::make_pair((size_t)2, false));
}

TEST_CASE(RLM3_WIFI_ServerConnect_Fail)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	ASSERT(!RLM3_WIFI_ServerConnect(2,"test-server", "test-port"));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_ServerDisconnect_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPCLOSE=2\r\n");
	SIM_RLM3_UART4_Receive("2,CLOSED\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	RLM3_WIFI_ServerDisconnect(2);
	ASSERT(!RLM3_WIFI_IsServerConnected(2));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 2);
	ASSERT(g_network_disconnect_calls.size() == 1);
	ASSERT(g_network_disconnect_calls.front() == std::make_pair((size_t)2, false));
}

TEST_CASE(RLM3_WIFI_ServerDisconnect_Fail)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPCLOSE=2\r\n");
	SIM_RLM3_UART4_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2,"test-server", "test-port");
	RLM3_WIFI_ServerDisconnect(2);
	ASSERT(RLM3_WIFI_IsServerConnected(2));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 1);
	ASSERT(g_network_disconnect_calls.empty());
}

TEST_CASE(RLM3_WIFI_Transmit_HappyCase)
{
	uint8_t buffer[] = { 'a', 'b', 'c', 'd', 'c', 'b', 'a' };

	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSEND=2,7\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Receive("> \r\n");
	SIM_RLM3_UART4_Transmit("abcdcba");
	SIM_RLM3_UART4_Receive("Recv 7 bytes\r\n");
	SIM_RLM3_UART4_Receive("SEND OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	ASSERT(RLM3_WIFI_Transmit(2, buffer, sizeof(buffer)));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 1);
}

TEST_CASE(RLM3_WIFI_Transmit2_HappyCase)
{
	uint8_t bufferA[] = { 'a', 'b', 'c' };
	uint8_t bufferB[] = { 'd', 'c', 'b', 'a' };

	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSEND=2,7\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Receive("> \r\n");
	SIM_RLM3_UART4_Transmit("abc");
	SIM_RLM3_UART4_Transmit("dcba");
	SIM_RLM3_UART4_Receive("Recv 7 bytes\r\n");
	SIM_RLM3_UART4_Receive("SEND OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	ASSERT(RLM3_WIFI_Transmit2(2, bufferA, sizeof(bufferA), bufferB, sizeof(bufferB)));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 1);
}

TEST_CASE(RLM3_WIFI_Transmit_Empty)
{
	uint8_t buffer[] = { 'a', 'b', 'c', 'd', 'c', 'b', 'a' };

	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	ASSERT(!RLM3_WIFI_Transmit(2, buffer, 0));
}

TEST_CASE(RLM3_WIFI_Transmit_MaxSize)
{
	constexpr size_t BUFFER_SIZE = 1024;
	uint8_t buffer[BUFFER_SIZE + 1] = { 0 };
	for (size_t i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 'a';

	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSEND=2,1024\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Receive("> \r\n");
	SIM_RLM3_UART4_Transmit((const char*)buffer);
	SIM_RLM3_UART4_Receive("Recv 1024 bytes\r\n");
	SIM_RLM3_UART4_Receive("SEND OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	ASSERT(RLM3_WIFI_Transmit(2, buffer, BUFFER_SIZE));
}

TEST_CASE(RLM3_WIFI_Transmit_OverSize)
{
	constexpr size_t BUFFER_SIZE = 1025;
	uint8_t buffer[BUFFER_SIZE + 1] = { 0 };
	for (size_t i = 0; i < BUFFER_SIZE; i++)
		buffer[i] = 'a';

	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2,"test-server", "test-port");
	ASSERT(!RLM3_WIFI_Transmit(2,buffer, sizeof(buffer)));
}

TEST_CASE(RLM3_WIFI_Receive_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_RLM3_UART4_Receive("WIFI CONNECTED\r\n");
	SIM_RLM3_UART4_Receive("WIFI GOT IP\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_RLM3_UART4_Receive("2,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_AddDelay(100);
	SIM_RLM3_UART4_Receive("+IPD,2,5:abcde\r\n");


	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	while (g_recv_buffer_count < 5)
		RLM3_Take();
	ASSERT(std::strncmp((const char*)g_recv_buffer_data, "abcde", 5) == 0);
}

TEST_CASE(RLM3_WIFI_LocalNetworkEnable_HappyCase)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWMODE_CUR=3\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPAP_CUR=\"1.2.3.4\"\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWSAP_CUR=\"test-local-ssid\",\"test-local-password\",1,3,4,0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSERVER=1,test-local-service\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("test-local-ssid", "test-local-password", 4, "1.2.3.4", "test-local-service"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_LocalNetworkEnable_Connect)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWMODE_CUR=3\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPAP_CUR=\"1.2.3.4\"\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWSAP_CUR=\"test-local-ssid\",\"test-local-password\",1,3,4,0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSERVER=1,test-local-service\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_AddDelay(1000);
	SIM_RLM3_UART4_Receive("0,CONNECT\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("test-local-ssid", "test-local-password", 4, "1.2.3.4", "test-local-service"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	RLM3_Take();
	ASSERT(g_network_callback_count == 1);
	ASSERT(g_network_connect_calls.size() == 1);
	ASSERT(g_network_connect_calls.front() == std::make_pair((size_t)0, true));
}

TEST_CASE(RLM3_WIFI_LocalNetworkEnable_Closed)
{
	ExpectInit();
	SIM_RLM3_UART4_Transmit("AT+CWMODE_CUR=3\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPAP_CUR=\"1.2.3.4\"\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CWSAP_CUR=\"test-local-ssid\",\"test-local-password\",1,3,4,0\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_RLM3_UART4_Transmit("AT+CIPSERVER=1,test-local-service\r\n");
	SIM_RLM3_UART4_Receive("OK\r\n");
	SIM_AddDelay(1000);
	SIM_RLM3_UART4_Receive("0,CONNECT\r\n");
	SIM_RLM3_UART4_Receive("0,CLOSED\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("test-local-ssid", "test-local-password", 4, "1.2.3.4", "test-local-service"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	RLM3_Take();
	ASSERT(g_network_callback_count == 1);
	ASSERT(g_network_connect_calls.size() == 1);
	ASSERT(g_network_connect_calls.front() == std::make_pair((size_t)0, true));
	RLM3_Take();
	ASSERT(g_network_callback_count == 2);
	ASSERT(g_network_disconnect_calls.size() == 1);
	ASSERT(g_network_disconnect_calls.front() == std::make_pair((size_t)0, true));
}

TEST_SETUP(WIFI_TESTING_SETUP)
{
	g_client_thread = RLM3_GetCurrentTask();;
	g_recv_buffer_count = 0;
	g_network_callback_count = 0;
	g_network_connect_calls.clear();
	g_network_disconnect_calls.clear();
}
