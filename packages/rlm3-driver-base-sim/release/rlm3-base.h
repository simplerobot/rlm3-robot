#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


extern bool RLM3_IsIRQ();
extern bool RLM3_IsSchedulerRunning();

extern bool RLM3_IsDebugOutput();
extern void RLM3_DebugOutput(uint8_t c);
extern bool RLM3_DebugOutputFromISR(uint8_t c);

extern void RLM3_GetUniqueDeviceId(uint8_t id_out[12]);
extern uint32_t RLM3_GetUniqueDeviceShortId();

extern void SIM_SetUniqueDeviceId(const uint8_t id[12]);
extern void SIM_SetUniqueDeviceShortId(uint32_t id);

extern void SIM_ExpectDebugOutput(const char* str);


#ifdef __cplusplus
}
#endif
