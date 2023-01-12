#include "Test.h"
#include "rlm3-uart.h"
#include "rlm3-task.h"
#include "main.h"
#include <cctype>
#include "logger.h"

#include <vector>


LOGGER_ZONE(TEST);


static volatile RLM3_Task g_gps_task = nullptr;
static volatile uint8_t* g_gps_buffer_tx = nullptr;
static volatile uint8_t* g_gps_buffer_rx = nullptr;
static volatile size_t g_gps_size_tx = 0;
static volatile size_t g_gps_size_rx = 0;
static volatile size_t g_gps_error_count = 0;

static volatile RLM3_Task g_wifi_task = nullptr;
static volatile uint8_t* g_wifi_buffer_tx = nullptr;
static volatile uint8_t* g_wifi_buffer_rx = nullptr;
static volatile size_t g_wifi_size_tx = 0;
static volatile size_t g_wifi_size_rx = 0;
static volatile size_t g_wifi_error_count = 0;


TEST_CASE(UART2_Lifecycle_HappyCase)
{
	ASSERT(!RLM3_UART_GPS_IsInit());
	RLM3_UART_GPS_Init(115200);
	ASSERT(RLM3_UART_GPS_IsInit());
	RLM3_UART_GPS_Deinit();
	ASSERT(!RLM3_UART_GPS_IsInit());
}

TEST_CASE(UART4_Lifecycle_HappyCase)
{
	ASSERT(!RLM3_UART_WIFI_IsInit());
	RLM3_UART_WIFI_Init(115200);
	ASSERT(RLM3_UART_WIFI_IsInit());
	RLM3_UART_WIFI_Deinit();
	ASSERT(!RLM3_UART_WIFI_IsInit());
}

TEST_CASE(UART2_Transmit_HappyCase)
{
	uint8_t buffer[26];
	for (size_t i = 0; i < 26; i++)
		buffer[i] = 'A' + i;
	RLM3_UART_GPS_Init(115200);

	g_gps_task = RLM3_Task_GetCurrent();
	g_gps_buffer_tx = buffer;
	g_gps_size_tx = 26;

	RLM3_UART_GPS_Send();
	for (size_t i = 0; i < 10 && g_gps_size_tx != 0; i++)
		RLM3_Task_TakeWithTimeout(0);
	g_gps_task = nullptr;

	RLM3_UART_GPS_Deinit();

	ASSERT(g_gps_size_tx == 0);
	ASSERT(g_gps_error_count == 0);
}

TEST_CASE(UART4_Transmit_HappyCase)
{
	uint8_t buffer[26];
	for (size_t i = 0; i < 26; i++)
		buffer[i] = 'A' + i;
	RLM3_UART_WIFI_Init(115200);

	g_wifi_task = RLM3_Task_GetCurrent();
	g_wifi_buffer_tx = buffer;
	g_wifi_size_tx = 26;

	RLM3_UART_WIFI_Send();
	for (size_t i = 0; i < 10 && g_wifi_size_tx != 0; i++)
		RLM3_Task_TakeWithTimeout(0);
	g_wifi_task = nullptr;

	RLM3_UART_WIFI_Deinit();

	ASSERT(g_wifi_size_tx == 0);
	ASSERT(g_wifi_error_count == 0);
}

TEST_CASE(UART2_Receive_HappyCase)
{
	RLM3_UART_GPS_Init(115200);

	// Enable GPS Reset Pin
	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_RESET);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPS_RESET_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPS_RESET_GPIO_Port, &GPIO_InitStruct);

	// Reset the GPS module.
	HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_RESET);
	RLM3_Task_Delay(10);
	HAL_GPIO_WritePin(GPS_RESET_GPIO_Port, GPS_RESET_Pin, GPIO_PIN_SET);
	RLM3_Task_Delay(1000);

	uint8_t command[] = { 0xA0, 0xA1, 0x00, 0x01, 0x10, 0x10, 0x0D, 0x0A }; // Query position update rate.
	g_gps_buffer_tx = command;
	g_gps_size_tx = sizeof(command);
	RLM3_UART_GPS_Send();

	uint8_t buffer[32];
	g_gps_buffer_rx = buffer;
	g_gps_size_rx = 32;

	RLM3_Task_Delay(500);

	RLM3_UART_GPS_Deinit();

	size_t count = 32 - g_gps_size_rx;

	ASSERT(count >= 9);
	const uint8_t expected[9] = { 0xA0, 0xA1, 0x00, 0x02, 0x83, 0x10, 0x93, 0x0D, 0x0A };
	for (size_t i = 0; i < 9; i++)
		ASSERT(buffer[i] == expected[i]);
	ASSERT(g_gps_error_count == 0);
}

extern void RLM3_UART_GPS_Receive_CB_ISR(uint8_t data)
{
	if (g_gps_buffer_rx != nullptr && g_gps_size_rx != 0)
	{
		*(g_gps_buffer_rx++) = data;
		if (--g_gps_size_rx == 0)
		{
			g_gps_buffer_rx = nullptr;
			RLM3_Task_GiveISR(g_gps_task);
		}
	}
}

extern bool RLM3_UART_GPS_Transmit_CB_ISR(uint8_t* data_to_send)
{
	if (g_gps_buffer_tx == nullptr || g_gps_size_tx == 0)
		return false;
	*data_to_send = *(g_gps_buffer_tx++);
	if (--g_gps_size_tx == 0)
	{
		g_gps_buffer_tx = nullptr;
		RLM3_Task_GiveISR(g_gps_task);
	}
	return true;
}

extern void RLM3_UART_GPS_Error_CB_ISR(uint32_t status_flags)
{
	g_gps_error_count++;
}

extern void RLM3_UART_WIFI_Receive_CB_ISR(uint8_t data)
{
	if (g_wifi_buffer_rx != nullptr && g_wifi_size_rx != 0)
	{
		*(g_wifi_buffer_rx++) = data;
		if (--g_wifi_size_rx == 0)
		{
			g_wifi_buffer_rx = nullptr;
			RLM3_Task_GiveISR(g_wifi_task);
		}
	}
}
extern bool RLM3_UART_WIFI_Transmit_CB_ISR(uint8_t* data_to_send)
{
	if (g_wifi_buffer_tx == nullptr || g_wifi_size_tx == 0)
		return false;
	*data_to_send = *(g_wifi_buffer_tx++);
	if (--g_wifi_size_tx == 0)
	{
		g_wifi_buffer_tx = nullptr;
		RLM3_Task_GiveISR(g_wifi_task);
	}
	return true;
}

extern void RLM3_UART_WIFI_Error_CB_ISR(uint32_t status_flags)
{
	g_wifi_error_count++;
}
