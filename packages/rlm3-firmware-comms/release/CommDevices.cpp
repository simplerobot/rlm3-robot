#include "CommDevices.hpp"
#include "rlm3-wifi.h"
#include "CommInput.hpp"
#include "CommOutput.hpp"


#ifdef MOCK_COMMS
	// There has got to be a better way to add mock stubs for testing.

	extern bool CommInput_PutMessageByteISR_MOCK(uint8_t byte);
	extern void CommInput_ResetPipeISR_MOCK();
	#define CommInput_PutMessageByteISR CommInput_PutMessageByteISR_MOCK
	#define CommInput_ResetPipeISR CommInput_ResetPipeISR_MOCK

	extern void CommOutput_OpenConnectionISR_MOCK(size_t link_id);
	extern void CommOutput_CloseConnectionISR_MOCK(size_t link_id);
	#define CommOutput_OpenConnectionISR CommOutput_OpenConnectionISR_MOCK
	#define CommOutput_CloseConnectionISR CommOutput_CloseConnectionISR_MOCK

#endif


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

extern void RLM3_WIFI_Receive_Callback(size_t link_id, uint8_t data)
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

extern void RLM3_WIFI_NetworkConnect_Callback(size_t link_id, bool local_connection)
{
	if (g_active_link != RLM3_WIFI_INVALID_LINK)
	{
		CommOutput_CloseConnectionISR(g_active_link);
	}

	g_active_link = link_id;
	CommInput_ResetPipeISR();
	CommOutput_OpenConnectionISR(link_id);
}

extern void RLM3_WIFI_NetworkDisconnect_Callback(size_t link_id, bool local_connection)
{
	if (g_active_link == link_id)
	{
		g_active_link = RLM3_WIFI_INVALID_LINK;
		CommInput_ResetPipeISR();
	}
}

