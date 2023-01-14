#include "CommOutput.hpp"
#include "rlm3-task.h"
#include "rlm3-wifi.h"
#include "CommMessage.hpp"
#include "Crc8.hpp"

#include "../../rlm-base/release/rlm-bytes.h"


static constexpr RLM3_Time SYNC_PACKET_DELAY_MS = 10 * 1000;

static RLM3_Task g_task = nullptr;
static volatile bool g_abort = false;
static volatile bool g_new_connection = false;
static volatile bool g_active_task = false;
static volatile bool g_old_connections[RLM3_WIFI_LINK_COUNT];
static volatile size_t g_link_id = RLM3_WIFI_INVALID_LINK;


static void SendVersionPacket(Crc8& crc)
{
	// TODO: Define version somewhere more flexible.
	MESSAGE_BODY_VERSION version = {};
	version.type = MESSAGE_TYPE_VERSION;
	version.id = hton_u32(0x524C4D33); // 'RLM3'
	version.version = hton_u32(0x00000001); // 0.0.0.1
	crc.add(&version, sizeof(version) - 1);
	version.crc = crc.get();

	size_t link_id = g_link_id;
	if (link_id < RLM3_WIFI_LINK_COUNT)
		RLM3_WIFI_Transmit(link_id, (const uint8_t*)&version, sizeof(version));
}

static void SendSyncPacket(Crc8& crc)
{
	MESSAGE_BODY_SYNC sync = {};
	sync.type = MESSAGE_TYPE_SYNC;
	sync.time = hton_u32(RLM3_Time_Get());
	crc.add(&sync, sizeof(sync) - 1);
	sync.crc = crc.get();

	size_t link_id = g_link_id;
	if (link_id < RLM3_WIFI_LINK_COUNT)
		RLM3_WIFI_Transmit(link_id, (const uint8_t*)&sync, sizeof(sync));
}

extern void CommOutput_Init()
{
	g_task = nullptr;
	g_abort = false;
	g_new_connection = false;
	g_active_task = false;
	for (auto& b : g_old_connections)
		b = false;
	g_link_id = RLM3_WIFI_INVALID_LINK;
}

extern void CommOutput_RunTask()
{
	g_task = RLM3_Task_GetCurrent();
	RLM3_Time last_sync_time = RLM3_Time_Get();
	Crc8 crc;

	while (!g_abort)
	{
		if (g_active_task)
		{
			g_active_task = false;
			if (g_new_connection)
			{
				g_new_connection = false;

				// Start the checksum over for this channel.
				crc.reset();

				SendVersionPacket(crc);
				SendSyncPacket(crc);
				last_sync_time = RLM3_Time_Get();
			}
			for (size_t link_id = 0; link_id < RLM3_WIFI_LINK_COUNT; link_id++)
			{
				if (g_old_connections[link_id])
				{
					g_old_connections[link_id] = false;
					if (RLM3_WIFI_IsServerConnected(link_id))
						RLM3_WIFI_ServerDisconnect(link_id);
				}
			}
		}
		else if (!RLM3_Task_TakeUntil(last_sync_time, SYNC_PACKET_DELAY_MS))
		{
			SendSyncPacket(crc);
			last_sync_time = RLM3_Time_Get();
		}
	}
	g_abort = false;
	g_task = nullptr;
}

extern void CommOutput_AbortTask()
{
	g_abort = true;
	RLM3_Task_Give(g_task);
}

extern void CommOutput_AbortTaskISR()
{
	g_abort = true;
	RLM3_Task_GiveISR(g_task);
}

extern void CommOutput_OpenConnectionISR(size_t link_id)
{
	g_link_id = link_id;
	g_new_connection = true;
	g_active_task = true;
	RLM3_Task_GiveISR(g_task);
}

extern void CommOutput_CloseConnectionISR(size_t link_id)
{
	if (link_id == g_link_id)
		g_link_id = RLM3_WIFI_INVALID_LINK;
	if (link_id < RLM3_WIFI_LINK_COUNT)
		g_old_connections[link_id] = true;
	g_active_task = true;
	RLM3_Task_GiveISR(g_task);
}
