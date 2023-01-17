#include "rlm3-uart.h"
#include "rlm3-sim.hpp"
#include "Test.hpp"
#include "logger.h"
#include <sstream>


LOGGER_ZONE(UART);


struct UartInfo
{
	bool is_active = false;
	uint32_t baud_rate = 0;
	const char* name = "";
	void (*ReceiveCallback)(uint8_t data);
	bool (*TransmitCallback)(uint8_t* data_to_send);
	void (*ErrorCallback)(uint32_t status_flags);
	bool is_transmitting = false;
};

static UartInfo UART_GPS;
static UartInfo UART_WIFI;


static void UART_Init(UartInfo& uart_info, uint32_t baud_rate)
{
	ASSERT(!SIM_IsISR());
	ASSERT(!uart_info.is_active);
	ASSERT(9600 <= baud_rate && baud_rate <= 10500000);
	uart_info.is_active = true;
	uart_info.baud_rate = baud_rate;
}

static void UART_Deinit(UartInfo& uart_info)
{
	ASSERT(!SIM_IsISR());
	ASSERT(uart_info.is_active);
	uart_info.is_active = false;
}

static bool UART_IsInit(UartInfo& uart_info)
{
	ASSERT(!SIM_IsISR());
	return uart_info.is_active;
}

static void UART_EnsureTransmit(UartInfo& uart_info)
{
	ASSERT(!SIM_IsISR());
	ASSERT(uart_info.is_active);
	uart_info.is_transmitting = true;
}

static uint32_t UART_GetBaudrate(UartInfo& uart_info)
{
	ASSERT(!SIM_IsISR());
	return uart_info.baud_rate;
}

static void UART_Transmit(UartInfo& u, const std::string& str)
{
	UartInfo* uart_info = &u;
	SIM_AddInterrupt([=]() {
		ASSERT(uart_info->is_active);
		if (!uart_info->is_transmitting)
			FAIL("%s transmit expected, but RLM3_%s_EnsureTransmit not called.", uart_info->name, uart_info->name);
		for (size_t i = 0; i < str.length(); i++)
		{
			uint8_t expected = str[i];
			uint8_t transmit_callback_value = 0;
			if (!uart_info->TransmitCallback(&transmit_callback_value))
				FAIL("%s transmit expected '%s' but only got %zd characters.", uart_info->name, SIM_SafeString(str).c_str(), i);
			if (transmit_callback_value != expected)
			{
				std::ostringstream full_actual;
				for (size_t j = 0; j < i; j++)
					full_actual.put(str[j]);
				full_actual.put(transmit_callback_value);
				while (uart_info->TransmitCallback(&transmit_callback_value) && std::isprint(transmit_callback_value))
					full_actual.put(transmit_callback_value);
				FAIL("%s transmit expected '%s' but got '%s'.", uart_info->name, SIM_SafeString(str).c_str(), SIM_SafeString(full_actual.str()).c_str());
			}
		}
		uint8_t transmit_callback_value = 0;
		ASSERT(!uart_info->TransmitCallback(&transmit_callback_value));
	});
}

static void UART_Receive(UartInfo& u, const std::string& str)
{
	UartInfo* uart_info = &u;
	SIM_AddInterrupt([=]() {
		ASSERT(uart_info->is_active);
		for (char c : str)
			uart_info->ReceiveCallback(c);
	});
}

static void UART_Transmit(UartInfo& u, const char* s)
{
	UART_Transmit(u, std::string(s));
}

static void UART_Receive(UartInfo& u, const char* s)
{
	UART_Receive(u, std::string(s));
}

static void UART_Transmit(UartInfo& u, const uint8_t* data, size_t size)
{
	UART_Transmit(u, std::string(data, data + size));
}

static void UART_Receive(UartInfo& u, const uint8_t* data, size_t size)
{
	UART_Receive(u, std::string(data, data + size));
}

extern void UART_Error(UartInfo& u, uint32_t status_flags)
{
	UartInfo* uart_info = &u;
	SIM_AddInterrupt([=]() {
		ASSERT(uart_info->is_active);
		uart_info->ErrorCallback(status_flags);
	});
}

extern void RLM3_UART_GPS_Init(uint32_t baud_rate)
{
	UART_Init(UART_GPS, baud_rate);
}

extern void RLM3_UART_GPS_Deinit()
{
	UART_Deinit(UART_GPS);
}

extern bool RLM3_UART_GPS_IsInit()
{
	return UART_IsInit(UART_GPS);
}

extern void RLM3_UART_GPS_Send()
{
	UART_EnsureTransmit(UART_GPS);
}

extern uint32_t SIM_UART_GPS_GetBaudrate()
{
	return UART_GetBaudrate(UART_GPS);
}

extern void SIM_UART_GPS_Transmit(const char* str)
{
	UART_Transmit(UART_GPS, str);
}

extern void SIM_UART_GPS_Receive(const char* str)
{
	UART_Receive(UART_GPS, str);
}

extern void SIM_UART_GPS_TransmitRaw(const uint8_t* data, size_t size)
{
	UART_Transmit(UART_GPS, data, size);
}

extern void SIM_UART_GPS_ReceiveRaw(const uint8_t* data, size_t size)
{
	UART_Receive(UART_GPS, data, size);
}

extern void SIM_UART_GPS_Error(uint32_t status_flags)
{
	UART_Error(UART_GPS, status_flags);
}

extern void RLM3_UART_WIFI_Init(uint32_t baud_rate)
{
	UART_Init(UART_WIFI, baud_rate);
}

extern void RLM3_UART_WIFI_Deinit()
{
	UART_Deinit(UART_WIFI);
}

extern bool RLM3_UART_WIFI_IsInit()
{
	return UART_IsInit(UART_WIFI);
}

extern void RLM3_UART_WIFI_Send()
{
	UART_EnsureTransmit(UART_WIFI);
}

extern uint32_t SIM_UART_WIFI_GetBaudrate()
{
	return UART_GetBaudrate(UART_WIFI);
}

extern void SIM_UART_WIFI_Transmit(const char* str)
{
	UART_Transmit(UART_WIFI, str);
}

extern void SIM_UART_WIFI_Receive(const char* str)
{
	UART_Receive(UART_WIFI, str);
}

extern void SIM_UART_WIFI_TransmitRaw(const uint8_t* data, size_t size)
{
	UART_Transmit(UART_WIFI, data, size);
}

extern void SIM_UART_WIFI_ReceiveRaw(const uint8_t* data, size_t size)
{
	UART_Receive(UART_WIFI, data, size);
}

extern void SIM_UART_WIFI_Error(uint32_t status_flags)
{
	UART_Error(UART_WIFI, status_flags);
}

extern __attribute__((weak)) void RLM3_UART_GPS_Receive_CB_ISR(uint8_t data)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern __attribute__((weak)) bool RLM3_UART_GPS_Transmit_CB_ISR(uint8_t* data_to_send)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
	return false;
}

extern __attribute__((weak)) void RLM3_UART_GPS_Error_CB_ISR(uint32_t status_flags)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern __attribute__((weak)) void RLM3_UART_WIFI_Receive_CB_ISR(uint8_t data)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern __attribute__((weak)) bool RLM3_UART_WIFI_Transmit_CB_ISR(uint8_t* data_to_send)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
	return false;
}

extern __attribute__((weak)) void RLM3_UART_WIFI_Error_CB_ISR(uint32_t status_flags)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

TEST_SETUP(UART_Init)
{
	UART_GPS.is_active = false;
	UART_GPS.baud_rate = 0;
	UART_GPS.name = "UART2";
	UART_GPS.ReceiveCallback = RLM3_UART_GPS_Receive_CB_ISR;
	UART_GPS.TransmitCallback = RLM3_UART_GPS_Transmit_CB_ISR;
	UART_GPS.ErrorCallback = RLM3_UART_GPS_Error_CB_ISR;
	UART_GPS.is_transmitting = false;

	UART_WIFI.is_active = false;
	UART_WIFI.baud_rate = 0;
	UART_WIFI.name = "UART4";
	UART_WIFI.ReceiveCallback = RLM3_UART_WIFI_Receive_CB_ISR;
	UART_WIFI.TransmitCallback = RLM3_UART_WIFI_Transmit_CB_ISR;
	UART_WIFI.ErrorCallback = RLM3_UART_WIFI_Error_CB_ISR;
	UART_WIFI.is_transmitting = false;
}
