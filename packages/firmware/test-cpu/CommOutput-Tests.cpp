#include "../../firmware/main/CommOutput.hpp"
#include "Test.hpp"
#include "rlm3-sim.hpp"
#include "rlm3-wifi.h"
#include "Crc8.hpp"


TEST_CASE(CommOutput_Abort)
{
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });

	CommOutput_Init();
	CommOutput_RunTask();
}

TEST_CASE(CommOutput_AbortRestart)
{
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });

	CommOutput_Init();
	CommOutput_RunTask();
	CommOutput_RunTask();
}

static void ExpectMessage(size_t link_id, Crc8& crc, std::initializer_list<uint8_t> data)
{
	for (uint8_t x : data)
	{
		SIM_WIFI_TransmitByte(link_id, x);
		crc.add(x);
	}
	SIM_WIFI_TransmitByte(link_id, crc.get());
}

TEST_CASE(CommOutput_HappyCase)
{
	Crc8 crc;
	SIM_AddInterrupt([] { CommOutput_OpenConnectionISR(3); });
	ExpectMessage(3, crc, {
		2, // MESSAGE_TYPE_VERSION
		0x52, 0x4C, 0x4D, 0x33, // 'RLM3'
		0x00, 0x00, 0x00, 0x01, // 0.0.0.1
	});
	ExpectMessage(3, crc, {
		3, // MESSAGE_TYPE_SYNC
		0x12, 0x34, 0x56, 0x78, // time
	});
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });
	SIM_WIFI_ServerConnect(3);
	RLM3_Task_Delay(0x12345678);

	CommOutput_Init();
	CommOutput_RunTask();
}

TEST_CASE(CommOutput_RegularSync)
{
	Crc8 crc;
	SIM_AddInterrupt([] { CommOutput_OpenConnectionISR(3); });
	ExpectMessage(3, crc, {
		2, // MESSAGE_TYPE_VERSION
		0x52, 0x4C, 0x4D, 0x33, // 'RLM3'
		0x00, 0x00, 0x00, 0x01, // 0.0.0.1
	});
	ExpectMessage(3, crc, {
		3, // MESSAGE_TYPE_SYNC
		0x12, 0x34, 0x56, 0x78, // time
	});
	ExpectMessage(3, crc, {
		3, // MESSAGE_TYPE_SYNC
		0x12, 0x34, 0x7D, 0x88, // time + 10,000
	});
	ExpectMessage(3, crc, {
		3, // MESSAGE_TYPE_SYNC
		0x12, 0x34, 0xA4, 0x98, // time + 20,000
	});
	SIM_AddDelay(25000);
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });
	SIM_WIFI_ServerConnect(3);
	RLM3_Task_Delay(0x12345678);

	CommOutput_Init();
	CommOutput_RunTask();
}

TEST_CASE(CommOutput_Idle)
{
	Crc8 crc;
	SIM_AddDelay(25000);
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });

	CommOutput_Init();
	CommOutput_RunTask();
}

TEST_CASE(CommOutput_CloseConnection_HappyCase)
{
	Crc8 crc;
	SIM_AddInterrupt([] { CommOutput_CloseConnectionISR(2); });
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });
	SIM_WIFI_ServerConnect(2);

	CommOutput_Init();
	CommOutput_RunTask();
}

TEST_CASE(CommOutput_CloseConnection_NotOpen)
{
	Crc8 crc;
	SIM_AddInterrupt([] { CommOutput_CloseConnectionISR(3); });
	SIM_AddInterrupt([] { CommOutput_AbortTaskISR(); });

	CommOutput_Init();
	CommOutput_RunTask();
}

TEST_SETUP(CommOutput_Setup)
{
	SIM_WIFI_IgnoreGpio();
}
