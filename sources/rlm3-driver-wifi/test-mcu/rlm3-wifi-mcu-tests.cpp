#include "Test.h"
#include "rlm3-wifi.h"
#include "rlm3-task.h"
#include "rlm3-gpio.h"
#include "logger.h"
#include <cstring>
#include <initializer_list>


LOGGER_ZONE(TEST);


static volatile size_t g_recv_count = 0;
static uint8_t g_recv_buffer[32];
static const size_t k_no_network = (size_t)-1;
static volatile size_t g_local_network = k_no_network;

extern void RLM3_WIFI_Receive_CB_ISR(size_t link_id, uint8_t data)
{
	if (g_recv_count < sizeof(g_recv_buffer))
		g_recv_buffer[g_recv_count] = data;
	g_recv_count++;
}

extern void RLM3_WIFI_NetworkConnect_CB_ISR(size_t link_id, bool local_connection)
{
	if (!local_connection)
		return;
	ASSERT(g_local_network == k_no_network);
	g_local_network = link_id;
}

extern void RLM3_WIFI_NetworkDisconnect_CB_ISR(size_t link_id, bool local_connection)
{
	if (!local_connection)
		return;
	ASSERT(link_id == g_local_network);
	g_local_network = k_no_network;
}


TEST_CASE(RLM3_WIFI_Lifecycle)
{
	RLM3_GPIO_Init();
	ASSERT(!RLM3_WIFI_IsInit());
	ASSERT(RLM3_WIFI_Init());
	ASSERT(RLM3_WIFI_IsInit());
	RLM3_WIFI_Deinit();
	ASSERT(!RLM3_WIFI_IsInit());
}

TEST_CASE(RLM3_WIFI_GetVersion_HappyCase)
{
	RLM3_GPIO_Init();
	ASSERT(RLM3_WIFI_Init());
	uint32_t at_version = 0;
	uint32_t sdk_version = 0;
	ASSERT(RLM3_WIFI_GetVersion(&at_version, &sdk_version));
	LOG_ALWAYS("WIFI AT_VERSION: %lX SDK_VERSION: %lX", at_version, sdk_version);
	ASSERT(at_version != 0);
	ASSERT(sdk_version != 0);
	RLM3_WIFI_Deinit();
}

TEST_CASE(RLM3_WIFI_NetworkConnect_HappyCase)
{
	RLM3_GPIO_Init();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	ASSERT(RLM3_WIFI_Init());
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	ASSERT(RLM3_WIFI_NetworkConnect("simplerobots", "gKFAED2xrf258vEp"));
	ASSERT(RLM3_WIFI_IsNetworkConnected());
	RLM3_WIFI_NetworkDisconnect();
	ASSERT(!RLM3_WIFI_IsNetworkConnected());
	RLM3_WIFI_Deinit();
}

TEST_CASE(RLM3_WIFI_ServerConnect_HappyCase)
{
	RLM3_GPIO_Init();
	ASSERT(RLM3_WIFI_Init());
	ASSERT(RLM3_WIFI_NetworkConnect("simplerobots", "gKFAED2xrf258vEp"));
	ASSERT(!RLM3_WIFI_IsServerConnected(2));
	ASSERT(RLM3_WIFI_ServerConnect(2, "www.google.com", "80"));
	ASSERT(RLM3_WIFI_IsServerConnected(2));
	RLM3_WIFI_ServerDisconnect(2);
	ASSERT(!RLM3_WIFI_IsServerConnected(2));
	RLM3_WIFI_Deinit();
}

TEST_CASE(RLM3_WIFI_SendReceive)
{
	RLM3_GPIO_Init();
	ASSERT(RLM3_WIFI_Init());
	ASSERT(RLM3_WIFI_NetworkConnect("simplerobots", "gKFAED2xrf258vEp"));
	ASSERT(!RLM3_WIFI_IsServerConnected(2));
	ASSERT(RLM3_WIFI_ServerConnect(2, "www.google.com", "80"));
	ASSERT(RLM3_WIFI_IsServerConnected(2));
	g_recv_count = 0;
	const char* command = "GET /\r\n";
	RLM3_WIFI_Transmit(2, (const uint8_t*)command, std::strlen(command));
	RLM3_Task_Delay(1000);
	RLM3_WIFI_ServerDisconnect(2);
	ASSERT(!RLM3_WIFI_IsServerConnected(2));
	RLM3_WIFI_Deinit();

	ASSERT(g_recv_count > 1024);
	ASSERT(std::strncmp((char*)g_recv_buffer, "HTTP/1.0 200 OK", 15) == 0);
}

TEST_CASE(RLM3_WIFI_LocalServer_HappyCase)
{
	RLM3_GPIO_Init();
	ASSERT(RLM3_WIFI_Init());
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	ASSERT(RLM3_WIFI_LocalNetworkEnable("SimpleRobots1234", "2o70jQcB", 2, "192.168.1.1", "80"));
	ASSERT(RLM3_WIFI_IsLocalNetworkEnabled());
	RLM3_WIFI_LocalNetworkDisable();
	ASSERT(!RLM3_WIFI_IsLocalNetworkEnabled());
	RLM3_WIFI_Deinit();
}

TEST_SETUP(WIFI_LOGGING)
{
	logger_set_level("WIFI", LOGGER_LEVEL_TRACE);
}

TEST_TEARDOWN(WIFI_Teardown)
{
	if (RLM3_WIFI_IsInit())
		RLM3_WIFI_Deinit();
	if (RLM3_GPIO_IsInit())
		RLM3_GPIO_DeInit();
}
