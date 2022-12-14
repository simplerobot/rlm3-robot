#include "CommunicationTask.hpp"
#include "logger.h"
#include "rlm3-wifi.h"
#include "rlm3-task.h"
#include "RingBuffer.hpp"


LOGGER_ZONE(COMMS); // @suppress("Unused variable declaration in file scope")


static RLM3_Task g_task = nullptr;

static constexpr size_t SYNC_MESSAGE_FREQUENCY_MS = 1000;

static constexpr size_t INVALID_LINK = -1;
static volatile size_t g_active_link = INVALID_LINK;
static RingBuffer<64> g_input;

static volatile bool g_request_close = false;
static volatile bool g_links_to_close[RLM3_WIFI_LINK_COUNT] = {};
static volatile bool g_new_connection = false;


static void CloseLinkFromISR(size_t link_id)
{
	// Ask the client to close the link
	g_links_to_close[link_id] = true;
	g_request_close = true;
	// If this is the link we were using, close it.
	if (link_id == g_active_link)
	{
		g_active_link = INVALID_LINK;
		g_input.reset_isr(g_task);
	}
	// Make sure the client is running to do the cleanup.
	RLM3_GiveFromISR(g_task);
}

extern void RLM3_WIFI_Receive_Callback(size_t link_id, uint8_t data)
{
	// If we are receiving data from an inactive link, close it instead of just blindly ignoring the data.
	if (link_id != g_active_link)
	{
		CloseLinkFromISR(link_id);
		return;
	}

	// Send the data to the client thread.
	if (!g_input.put_isr(data, g_task))
	{
		LOG_ERROR("Overflow");
		CloseLinkFromISR(link_id);
		return;
	}

	// Success.
}

extern void RLM3_WIFI_NetworkConnect_Callback(size_t link_id, bool local_connection)
{
	// If we had an open connection, close it.
	if (g_active_link != INVALID_LINK)
	{
		CloseLinkFromISR(g_active_link);
	}

	g_active_link = link_id;
	g_new_connection = true;
	RLM3_GiveFromISR(g_task);
}

extern void RLM3_WIFI_NetworkDisconnect_Callback(size_t link_id, bool local_connection)
{
	g_links_to_close[link_id] = false;
	if (g_active_link == link_id)
		g_active_link = INVALID_LINK;
	RLM3_GiveFromISR(g_task);
}

static void RunFirmwareMainLoop()
{
	RLM3_Time last_sync_time;

	while (true)
	{
		if (g_input.is_reset())
		{
			// Erase any pending data from the ring buffer.  (This is due to a dropped connection, new connection, or overflow.)
			g_input.clear_reset();
		}
		else if (g_request_close)
		{
			// Close any connections the server wants closed.
			g_request_close = false;
			for (size_t i = 0; i < RLM3_WIFI_LINK_COUNT; i++)
			{
				if (g_links_to_close[i])
				{
					g_links_to_close[i] = false;
					RLM3_WIFI_ServerDisconnect(i);
				}
			}
		}
		else if (g_active_link == INVALID_LINK)
		{
			// Just wait for an active connection.
			RLM3_Take();
		}
		else if (g_new_connection)
		{
			g_new_connection = false;
			// Send a version string.
			// Anything else?
			// Send a sync message.
			last_sync_time = RLM3_GetCurrentTime();
		}
		else if (RLM3_GetCurrentTime() - last_sync_time > SYNC_MESSAGE_FREQUENCY_MS)
		{
			// It's time to send an sync message.
		}
		else
		{
			// Wait for a message from the server.
		}
	}
}

extern void RunFimwareComms()
{
	g_task = RLM3_GetCurrentTask();

	LOG_ALWAYS("Startup");
	if (RLM3_WIFI_Init())
	{
		if (RLM3_WIFI_LocalNetworkEnable("RLM3", "ABCD1234", 2, "10.168.154.1", "37649"))
		{
			RunFirmwareMainLoop();
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

