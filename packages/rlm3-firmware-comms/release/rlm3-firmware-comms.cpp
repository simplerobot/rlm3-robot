#include "logger.h"
#include "rlm3-wifi.h"
#include "rlm3-task.h"
#include "rlm3-firmware-comms.hpp"


LOGGER_ZONE(COMMS); // @suppress("Unused variable declaration in file scope")


#define RLM3_WIFI_LINK_COUNT (5)

static bool g_quit = false;
static RLM3_Task g_task = nullptr;


extern void RLM3_WIFI_Receive_Callback(size_t link_id, uint8_t data)
{
	LOG_ALWAYS("%d:%c", (int)link_id, (data >= ' ' && data <= '~') ? data : '?');

	if (data == 'X')
	{
		g_quit = true;
		RLM3_Give(g_task);
	}
}

extern void RLM3_WIFI_NetworkConnect_Callback(size_t link_id, bool local_connection)
{
	LOG_ALWAYS("%d+", (int)link_id);
}

extern void RLM3_WIFI_NetworkDisconnect_Callback(size_t link_id, bool local_connection)
{
	LOG_ALWAYS("%d-", (int)link_id);
}

extern void RunFimwareComms()
{
	g_task = RLM3_GetCurrentTask();
	RLM3_Time start_time = RLM3_GetCurrentTime();

	LOG_ALWAYS("Startup");
	if (RLM3_WIFI_Init())
	{
		if (RLM3_WIFI_LocalNetworkEnable("RLM3", "ABCD1234", 2, "10.168.154.1", "37649"))
		{
			while (!g_quit && RLM3_TakeUntil(start_time, 300 * 1000))
				LOG_ALWAYS("WAITING");
			RLM3_WIFI_LocalNetworkDisable();
		}
		else
			LOG_ALWAYS("LocalNetworkEnable failed");
		RLM3_WIFI_Deinit();
	}
	else
		LOG_ALWAYS("Init failed");
	LOG_ALWAYS("Shutdown");
}

