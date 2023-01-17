#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


#define RLM3_EXTERNAL_MEMORY_DATA ((uint8_t*)0xD0000000)
#define RLM3_EXTERNAL_MEMORY_SIZE ((size_t)(8 * 1024 * 1024))


extern void RLM3_Mem_Init();
extern void RLM3_Mem_Deinit();
extern bool RLM3_Mem_IsInit();


#ifdef __cplusplus
}
#endif


