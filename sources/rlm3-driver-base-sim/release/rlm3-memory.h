#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


#define RLM3_EXTERNAL_MEMORY_DATA (SIM_MEMORY_GetBaseAddress())
#define RLM3_EXTERNAL_MEMORY_SIZE (8 * 1024 * 1024)


extern void RLM3_Mem_Init();
extern void RLM3_Mem_Deinit();
extern bool RLM3_Mem_IsInit();


extern uint8_t* SIM_MEMORY_GetBaseAddress();


#ifdef __cplusplus
}
#endif


