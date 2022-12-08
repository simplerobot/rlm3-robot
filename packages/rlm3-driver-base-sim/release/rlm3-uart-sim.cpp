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

static UartInfo UART2;
static UartInfo UART4;


static void UART_Init(UartInfo& uart_info, uint32_t baud_rate)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(!uart_info.is_active);
	ASSERT(9600 <= baud_rate && baud_rate <= 10500000);
	uart_info.is_active = true;
	uart_info.baud_rate = baud_rate;
}

static void UART_Deinit(UartInfo& uart_info)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(uart_info.is_active);
	uart_info.is_active = false;
}

static bool UART_IsInit(UartInfo& uart_info)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	return uart_info.is_active;
}

static void UART_EnsureTransmit(UartInfo& uart_info)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
	ASSERT(uart_info.is_active);
	uart_info.is_transmitting = true;
}

static uint32_t UART_GetBaudrate(UartInfo& uart_info)
{
	ASSERT(!SIM_RLM3_Is_IRQ());
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

extern void RLM3_UART2_Init(uint32_t baud_rate)
{
	UART_Init(UART2, baud_rate);
}

extern void RLM3_UART2_Deinit()
{
	UART_Deinit(UART2);
}

extern bool RLM3_UART2_IsInit()
{
	return UART_IsInit(UART2);
}

extern void RLM3_UART2_EnsureTransmit()
{
	UART_EnsureTransmit(UART2);
}

extern uint32_t SIM_RLM3_UART2_GetBaudrate()
{
	return UART_GetBaudrate(UART2);
}

extern void SIM_RLM3_UART2_Transmit(const char* str)
{
	UART_Transmit(UART2, str);
}

extern void SIM_RLM3_UART2_Receive(const char* str)
{
	UART_Receive(UART2, str);
}

extern void SIM_RLM3_UART2_TransmitRaw(const uint8_t* data, size_t size)
{
	UART_Transmit(UART2, data, size);
}

extern void SIM_RLM3_UART2_ReceiveRaw(const uint8_t* data, size_t size)
{
	UART_Receive(UART2, data, size);
}

extern void SIM_RLM3_UART2_Error(uint32_t status_flags)
{
	UART_Error(UART2, status_flags);
}

extern void RLM3_UART4_Init(uint32_t baud_rate)
{
	UART_Init(UART4, baud_rate);
}

extern void RLM3_UART4_Deinit()
{
	UART_Deinit(UART4);
}

extern bool RLM3_UART4_IsInit()
{
	return UART_IsInit(UART4);
}

extern void RLM3_UART4_EnsureTransmit()
{
	UART_EnsureTransmit(UART4);
}

extern uint32_t SIM_RLM3_UART4_GetBaudrate()
{
	return UART_GetBaudrate(UART4);
}

extern void SIM_RLM3_UART4_Transmit(const char* str)
{
	UART_Transmit(UART4, str);
}

extern void SIM_RLM3_UART4_Receive(const char* str)
{
	UART_Receive(UART4, str);
}

extern void SIM_RLM3_UART4_TransmitRaw(const uint8_t* data, size_t size)
{
	UART_Transmit(UART4, data, size);
}

extern void SIM_RLM3_UART4_ReceiveRaw(const uint8_t* data, size_t size)
{
	UART_Receive(UART4, data, size);
}

extern void SIM_RLM3_UART4_Error(uint32_t status_flags)
{
	UART_Error(UART4, status_flags);
}

extern __attribute((weak)) void RLM3_UART2_ReceiveCallback(uint8_t data)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern __attribute((weak)) bool RLM3_UART2_TransmitCallback(uint8_t* data_to_send)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
	return false;
}

extern __attribute((weak)) void RLM3_UART2_ErrorCallback(uint32_t status_flags)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern __attribute((weak)) void RLM3_UART4_ReceiveCallback(uint8_t data)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

extern __attribute((weak)) bool RLM3_UART4_TransmitCallback(uint8_t* data_to_send)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
	return false;
}

extern __attribute((weak)) void RLM3_UART4_ErrorCallback(uint32_t status_flags)
{
	// DO NOT MODIFIY THIS FUNCTION.  Override it by declaring a non-weak version in your project files.
	ASSERT(false);
}

TEST_SETUP(UART_Init)
{
	UART2.is_active = false;
	UART2.baud_rate = 0;
	UART2.name = "UART2";
	UART2.ReceiveCallback = RLM3_UART2_ReceiveCallback;
	UART2.TransmitCallback = RLM3_UART2_TransmitCallback;
	UART2.ErrorCallback = RLM3_UART2_ErrorCallback;
	UART2.is_transmitting = false;

	UART4.is_active = false;
	UART4.baud_rate = 0;
	UART4.name = "UART4";
	UART4.ReceiveCallback = RLM3_UART4_ReceiveCallback;
	UART4.TransmitCallback = RLM3_UART4_TransmitCallback;
	UART4.ErrorCallback = RLM3_UART4_ErrorCallback;
	UART4.is_transmitting = false;
}
