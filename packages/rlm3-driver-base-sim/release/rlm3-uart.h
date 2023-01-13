#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void RLM3_UART_GPS_Init(uint32_t baud_rate);
extern void RLM3_UART_GPS_Deinit();
extern bool RLM3_UART_GPS_IsInit();

extern void RLM3_UART_GPS_Send();

extern void RLM3_UART_GPS_Receive_CB_ISR(uint8_t data);
extern bool RLM3_UART_GPS_Transmit_CB_ISR(uint8_t* data_to_send);
extern void RLM3_UART_GPS_Error_CB_ISR(uint32_t status_flags);

extern uint32_t SIM_UART_GPS_GetBaudrate();
extern void SIM_UART_GPS_Transmit(const char* str);
extern void SIM_UART_GPS_Receive(const char* str);
extern void SIM_UART_GPS_TransmitRaw(const uint8_t* data, size_t size);
extern void SIM_UART_GPS_ReceiveRaw(const uint8_t* data, size_t size);
extern void SIM_UART_GPS_Error(uint32_t status_flags);


extern void RLM3_UART_WIFI_Init(uint32_t baud_rate);
extern void RLM3_UART_WIFI_Deinit();
extern bool RLM3_UART_WIFI_IsInit();

extern void RLM3_UART_WIFI_Send();

extern void RLM3_UART_WIFI_Receive_CB_ISR(uint8_t data);
extern bool RLM3_UART_WIFI_Transmit_CB_ISRk(uint8_t* data_to_send);
extern void RLM3_UART_WIFI_Error_CB_ISR(uint32_t status_flags);

extern uint32_t SIM_UART_WIFI_GetBaudrate();
extern void SIM_UART_WIFI_Transmit(const char* str);
extern void SIM_UART_WIFI_Receive(const char* str);
extern void SIM_UART_WIFI_TransmitRaw(const uint8_t* data, size_t size);
extern void SIM_UART_WIFI_ReceiveRaw(const uint8_t* data, size_t size);
extern void SIM_UART_WIFI_Error(uint32_t status_flags);

#ifdef __cplusplus
}
#endif
