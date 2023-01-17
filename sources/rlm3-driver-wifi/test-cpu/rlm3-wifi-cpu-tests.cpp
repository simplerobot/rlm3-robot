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


extern void RLM3_WIFI_Receive_CB_ISR(size_t link_id, uint8_t data)
{
	ASSERT(link_id == 2);
	if (g_recv_buffer_count < sizeof(g_recv_buffer_data))
		g_recv_buffer_data[g_recv_buffer_count] = data;
	g_recv_buffer_count++;
	RLM3_Task_GiveISR(g_client_thread);
}

extern void RLM3_WIFI_NetworkConnect_CB_ISR(size_t link_id, bool local_connection)
{
	g_network_callback_count++;
	g_network_connect_calls.emplace_back(link_id, local_connection);
	RLM3_Task_GiveISR(g_client_thread);
}

extern void RLM3_WIFI_NetworkDisconnect_CB_ISR(size_t link_id, bool local_connection)
{
	g_network_callback_count++;
	g_network_disconnect_calls.emplace_back(link_id, local_connection);
	RLM3_Task_GiveISR(g_client_thread);
}

TEST_CASE(RLM3_WIFI_IsInit_Uninitialized)
{
	ASSERT(!RLM3_WIFI_IsInit());
}

static void ExpectInit()
{
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");
	SIM_UART_WIFI_Receive("ATE0\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMODE=0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMUX=1\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWMODE_CUR=1\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWAUTOCONN=0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
}

TEST_CASE(RLM3_WIFI_Init_HappyCase)
{
	ExpectInit();
	RLM3_GPIO_Init();

	ASSERT(RLM3_WIFI_Init());

	ASSERT(RLM3_WIFI_IsInit());
	ASSERT(RLM3_UART_WIFI_IsInit());
	ASSERT(SIM_UART_WIFI_GetBaudrate() == 115200);
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_Init_PingTimeout)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_PingFailure)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nFAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_EchoTimeout)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_EchoFailure)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");
	SIM_UART_WIFI_Receive("ATE0\r\nFAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_SetTransmissionModeFailure)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");
	SIM_UART_WIFI_Receive("ATE0\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMODE=0\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_SetMultipleConnectionsFailure)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");
	SIM_UART_WIFI_Receive("ATE0\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMODE=0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMUX=1\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_SetAutoconnectFailure)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");
	SIM_UART_WIFI_Receive("ATE0\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMODE=0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMUX=1\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWMODE_CUR=1\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_Init_ManualConnectFailure)
{
	RLM3_GPIO_Init();
	SIM_UART_WIFI_Transmit("AT\r\n");
	SIM_UART_WIFI_Receive("AT\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("ATE0\r\n");
	SIM_UART_WIFI_Receive("ATE0\r\nOK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMODE=0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPMUX=1\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWMODE_CUR=1\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWAUTOCONN=0\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	ASSERT(!RLM3_WIFI_Init());
}

TEST_CASE(RLM3_WIFI_DeInit_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();

	RLM3_WIFI_Init();
	RLM3_WIFI_Deinit();
	ASSERT(!RLM3_WIFI_IsInit());
	ASSERT(!RLM3_UART_WIFI_IsInit());
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_GetVersion_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+GMR\r\n");
	SIM_UART_WIFI_Receive("AT version:255.254.253.252-dev(blah)\r\nSDK version:v251.250.249.248-ge7acblah\r\ncompile time(xxxx)\r\nBin version:2.1.0(Mini)\r\n\r\nOK\r\n");

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
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+GMR\r\n");

	RLM3_WIFI_Init();
	uint32_t at_version = 0;
	uint32_t sdk_version = 0;
	ASSERT(!RLM3_WIFI_GetVersion(&at_version, &sdk_version));
}

TEST_CASE(RLM3_WIFI_GetVersion_Failure)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+GMR\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	uint32_t at_version = 0;
	uint32_t sdk_version = 0;
	ASSERT(!RLM3_WIFI_GetVersion(&at_version, &sdk_version));
}

TEST_CASE(RLM3_WIFI_NetworkConnect_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_AddDelay(10);
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_AddDelay(10);
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_AddDelay(10);
	SIM_UART_WIFI_Receive("OK\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	ASSERT(RLM3_WIFI_NetworkConnect("test-sid", "test-pwd"));
	ASSERT(RLM3_WIFI_IsNetworkConnected());
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_NetworkConnect_Error)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_AddDelay(10);
	SIM_UART_WIFI_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	ASSERT(!RLM3_WIFI_NetworkConnect("test-sid", "test-pwd"));
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_NetworkDisconnect_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWQAP\r\n");
	SIM_UART_WIFI_Receive("WIFI DISCONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_NetworkDisconnect_Failure)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWQAP\r\n");
	SIM_UART_WIFI_Receive("WIFI DISCONNECT\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_NetworkDisconnect_NotConnected)
{
	RLM3_GPIO_Init();
	ExpectInit();

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
}

TEST_CASE(RLM3_WIFI_ServerConnect_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");

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
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	ASSERT(!RLM3_WIFI_ServerConnect(2,"test-server", "test-port"));
	ASSERT(g_recv_buffer_count == 0);
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_ServerDisconnect_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPCLOSE=2\r\n");
	SIM_UART_WIFI_Receive("2,CLOSED\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");

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
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPCLOSE=2\r\n");
	SIM_UART_WIFI_Receive("FAIL\r\n");

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

	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSEND=2,7\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Receive("> \r\n");
	SIM_UART_WIFI_Transmit("abcdcba");
	SIM_UART_WIFI_Receive("Recv 7 bytes\r\n");
	SIM_UART_WIFI_Receive("SEND OK\r\n");

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

	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSEND=2,7\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Receive("> \r\n");
	SIM_UART_WIFI_Transmit("abc");
	SIM_UART_WIFI_Transmit("dcba");
	SIM_UART_WIFI_Receive("Recv 7 bytes\r\n");
	SIM_UART_WIFI_Receive("SEND OK\r\n");

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

	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");

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

	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSEND=2,1024\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Receive("> \r\n");
	SIM_UART_WIFI_Transmit((const char*)buffer);
	SIM_UART_WIFI_Receive("Recv 1024 bytes\r\n");
	SIM_UART_WIFI_Receive("SEND OK\r\n");

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

	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");

	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2,"test-server", "test-port");
	ASSERT(!RLM3_WIFI_Transmit(2,buffer, sizeof(buffer)));
}

TEST_CASE(RLM3_WIFI_Receive_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWJAP_CUR=\"test-sid\",\"test-pwd\"\r\n");
	SIM_UART_WIFI_Receive("WIFI CONNECTED\r\n");
	SIM_UART_WIFI_Receive("WIFI GOT IP\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSTART=2,\"TCP\",\"test-server\",test-port\r\n");
	SIM_UART_WIFI_Receive("2,CONNECT\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_AddDelay(100);
	SIM_UART_WIFI_Receive("+IPD,2,5:abcde\r\n");


	RLM3_WIFI_Init();
	RLM3_WIFI_NetworkConnect("test-sid", "test-pwd");
	RLM3_WIFI_ServerConnect(2, "test-server", "test-port");
	while (g_recv_buffer_count < 5)
		RLM3_Task_Take();
	ASSERT(std::strncmp((const char*)g_recv_buffer_data, "abcde", 5) == 0);
}

TEST_CASE(RLM3_WIFI_LocalNetworkEnable_HappyCase)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWMODE_CUR=3\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPAP_CUR=\"1.2.3.4\"\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWSAP_CUR=\"test-local-ssid\",\"test-local-password\",1,3,4,0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSERVER=1,test-local-service\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("test-local-ssid", "test-local-password", 4, "1.2.3.4", "test-local-service"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(g_network_callback_count == 0);
}

TEST_CASE(RLM3_WIFI_LocalNetworkEnable_Connect)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWMODE_CUR=3\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPAP_CUR=\"1.2.3.4\"\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWSAP_CUR=\"test-local-ssid\",\"test-local-password\",1,3,4,0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSERVER=1,test-local-service\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_AddDelay(1000);
	SIM_UART_WIFI_Receive("0,CONNECT\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("test-local-ssid", "test-local-password", 4, "1.2.3.4", "test-local-service"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	RLM3_Task_Take();
	ASSERT(g_network_callback_count == 1);
	ASSERT(g_network_connect_calls.size() == 1);
	ASSERT(g_network_connect_calls.front() == std::make_pair((size_t)0, true));
}

TEST_CASE(RLM3_WIFI_LocalNetworkEnable_Closed)
{
	RLM3_GPIO_Init();
	ExpectInit();
	SIM_UART_WIFI_Transmit("AT+CWMODE_CUR=3\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPAP_CUR=\"1.2.3.4\"\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CWSAP_CUR=\"test-local-ssid\",\"test-local-password\",1,3,4,0\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_UART_WIFI_Transmit("AT+CIPSERVER=1,test-local-service\r\n");
	SIM_UART_WIFI_Receive("OK\r\n");
	SIM_AddDelay(1000);
	SIM_UART_WIFI_Receive("0,CONNECT\r\n");
	SIM_UART_WIFI_Receive("0,CLOSED\r\n");

	RLM3_WIFI_Init();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("test-local-ssid", "test-local-password", 4, "1.2.3.4", "test-local-service"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	RLM3_Task_Take();
	ASSERT(g_network_callback_count == 1);
	ASSERT(g_network_connect_calls.size() == 1);
	ASSERT(g_network_connect_calls.front() == std::make_pair((size_t)0, true));
	RLM3_Task_Take();
	ASSERT(g_network_callback_count == 2);
	ASSERT(g_network_disconnect_calls.size() == 1);
	ASSERT(g_network_disconnect_calls.front() == std::make_pair((size_t)0, true));
}

TEST_SETUP(WIFI_TESTING_SETUP)
{
	g_client_thread = RLM3_Task_GetCurrent();;
	g_recv_buffer_count = 0;
	g_network_callback_count = 0;
	g_network_connect_calls.clear();
	g_network_disconnect_calls.clear();
}
