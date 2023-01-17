#include "../../firmware/main/CommDevices.hpp"

#include "../../firmware/main/CommInput.hpp"
#include "../../firmware/main/CommOutput.hpp"
#include "rlm3-wifi.h"


static volatile size_t g_active_link;


extern bool CommDevices_Init()
{
	g_active_link = RLM3_WIFI_INVALID_LINK;

	if (!RLM3_WIFI_Init())
	{
		return false;
	}

	// TODO: We will want to generate the wifi name and password from the network name.
	if (!RLM3_WIFI_LocalNetworkEnable("RLM3", "ABCD1234", 2, "10.168.154.1", "37649"))
	{
		RLM3_WIFI_Deinit();
		return false;
	}

	return true;
}

extern void CommDevices_Deinit()
{
	RLM3_WIFI_LocalNetworkDisable();
	RLM3_WIFI_Deinit();
}

extern void RLM3_WIFI_Receive_CB_ISR(size_t link_id, uint8_t data)
{
	if (link_id != g_active_link)
	{
		CommOutput_CloseConnectionISR(link_id);
		return;
	}

	if (!CommInput_PutMessageByteISR(data))
	{
		g_active_link = RLM3_WIFI_INVALID_LINK;
		CommInput_ResetPipeISR();
		CommOutput_CloseConnectionISR(link_id);
		return;
	}

	// Successfully sent data to input task.
}

extern void RLM3_WIFI_NetworkConnect_CB_ISR(size_t link_id, bool local_connection)
{
	if (g_active_link != RLM3_WIFI_INVALID_LINK)
	{
		CommOutput_CloseConnectionISR(g_active_link);
	}

	g_active_link = link_id;
	CommInput_ResetPipeISR();
	CommOutput_OpenConnectionISR(link_id);
}

extern void RLM3_WIFI_NetworkDisconnect_CB_ISR(size_t link_id, bool local_connection)
{
	if (g_active_link == link_id)
	{
		g_active_link = RLM3_WIFI_INVALID_LINK;
		CommInput_ResetPipeISR();
	}
}

