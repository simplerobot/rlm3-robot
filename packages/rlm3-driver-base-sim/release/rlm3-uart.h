#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void RLM3_UART2_Init(uint32_t baud_rate);
extern void RLM3_UART2_Deinit();
extern bool RLM3_UART2_IsInit();

extern void RLM3_UART2_EnsureTransmit();

extern void RLM3_UART2_ReceiveCallback(uint8_t data);
extern bool RLM3_UART2_TransmitCallback(uint8_t* data_to_send);
extern void RLM3_UART2_ErrorCallback(uint32_t status_flags);

extern uint32_t SIM_RLM3_UART2_GetBaudrate();
extern void SIM_RLM3_UART2_Transmit(const char* str);
extern void SIM_RLM3_UART2_Receive(const char* str);
extern void SIM_RLM3_UART2_TransmitRaw(const uint8_t* data, size_t size);
extern void SIM_RLM3_UART2_ReceiveRaw(const uint8_t* data, size_t size);
extern void SIM_RLM3_UART2_Error(uint32_t status_flags);


extern void RLM3_UART4_Init(uint32_t baud_rate);
extern void RLM3_UART4_Deinit();
extern bool RLM3_UART4_IsInit();

extern void RLM3_UART4_EnsureTransmit();

extern void RLM3_UART4_ReceiveCallback(uint8_t data);
extern bool RLM3_UART4_TransmitCallback(uint8_t* data_to_send);
extern void RLM3_UART4_ErrorCallback(uint32_t status_flags);

extern uint32_t SIM_RLM3_UART4_GetBaudrate();
extern void SIM_RLM3_UART4_Transmit(const char* str);
extern void SIM_RLM3_UART4_Receive(const char* str);
extern void SIM_RLM3_UART4_TransmitRaw(const uint8_t* data, size_t size);
extern void SIM_RLM3_UART4_ReceiveRaw(const uint8_t* data, size_t size);
extern void SIM_RLM3_UART4_Error(uint32_t status_flags);

#ifdef __cplusplus
}
#endif
