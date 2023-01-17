#include "Test.hpp"
#include "rlm3-uart.h"
#include "rlm3-sim.hpp"


TEST_CASE(UART_Init_HappyCase)
{
	ASSERT(!RLM3_UART_GPS_IsInit());
	RLM3_UART_GPS_Init(115200);
	ASSERT(RLM3_UART_GPS_IsInit());
	ASSERT(SIM_UART_GPS_GetBaudrate() == 115200);
	RLM3_UART_GPS_Deinit();
	ASSERT(!RLM3_UART_GPS_IsInit());
}

TEST_CASE(Init_MinBaudrate)
{
	RLM3_UART_GPS_Init(9600);
}

TEST_CASE(Init_MaxBaudrate)
{
	RLM3_UART_GPS_Init(10500000);
}

TEST_CASE(Init_SmallBaudrate)
{
	ASSERT_ASSERTS(RLM3_UART_GPS_Init(9599));
}

TEST_CASE(Init_LargeBaudrate)
{
	ASSERT_ASSERTS(RLM3_UART_GPS_Init(10500001));
}

static const char* g_receive = nullptr;
static const char* g_transmit = nullptr;
static uint32_t g_error = 0;

extern void RLM3_UART_GPS_Receive_CB_ISR(uint8_t data)
{
	ASSERT(g_receive != nullptr && *g_receive != 0);
	ASSERT(*g_receive == data);
	g_receive++;
	if (*g_receive == 0)
	{
		g_receive = nullptr;
		SIM_Give();
	}
}

extern bool RLM3_UART_GPS_Transmit_CB_ISR(uint8_t* data_to_send)
{
	if (g_transmit == nullptr || *g_transmit == 0)
		return false;
	*data_to_send = *g_transmit;
	g_transmit++;
	if (*g_transmit == 0)
	{
		g_transmit = nullptr;
		SIM_Give();
	}
	return true;
}

extern void RLM3_UART_GPS_Error_CB_ISR(uint32_t status_flags)
{
	ASSERT(g_error == status_flags);
	g_error = 0;
	SIM_Give();
}

TEST_CASE(UART_TransmitReceiveError_HappyCase)
{
	SIM_UART_GPS_Transmit("GET /\r\n");
	SIM_AddDelay(10);
	SIM_UART_GPS_Receive("HTTP/1.0 200 OK\r\n");
	SIM_AddDelay(5);
	SIM_UART_GPS_Error(125);

	RLM3_Task_Delay(22);
	RLM3_UART_GPS_Init(115200);
	g_transmit = "GET /\r\n";
	g_receive = "HTTP/1.0 200 OK\r\n";
	g_error = 125;
	RLM3_UART_GPS_Send();
	while (g_transmit != nullptr)
		RLM3_Task_Take();
	ASSERT(RLM3_Time_Get() == 22 + 0);
	while (g_receive != nullptr)
		RLM3_Task_Take();
	ASSERT(RLM3_Time_Get() == 22 + 10);
	while (g_error != 0)
		RLM3_Task_Take();
	ASSERT(RLM3_Time_Get() == 22 + 15);
}

TEST_CASE(UART_TransmitReceiveRawError_HappyCase)
{
	SIM_UART_GPS_TransmitRaw((const uint8_t*)"GET /\r\n", 7);
	SIM_AddDelay(10);
	SIM_UART_GPS_ReceiveRaw((const uint8_t*)"HTTP/1.0 200 OK\r\n", 17);
	SIM_AddDelay(5);
	SIM_UART_GPS_Error(125);

	RLM3_Task_Delay(22);
	RLM3_UART_GPS_Init(115200);
	g_transmit = "GET /\r\n";
	g_receive = "HTTP/1.0 200 OK\r\n";
	g_error = 125;
	RLM3_UART_GPS_Send();
	while (g_transmit != nullptr)
		RLM3_Task_Take();
	ASSERT(RLM3_Time_Get() == 22 + 0);
	while (g_receive != nullptr)
		RLM3_Task_Take();
	ASSERT(RLM3_Time_Get() == 22 + 10);
	while (g_error != 0)
		RLM3_Task_Take();
	ASSERT(RLM3_Time_Get() == 22 + 15);
}

