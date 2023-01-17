#pragma once

#include "rlm3-base.h"


#ifdef __cplusplus
extern "C" {
#endif


extern void RLM3_Random_Init();
extern void RLM3_Random_Deinit();
extern bool RLM3_Random_IsInit();
extern void RLM3_Random_CB_ISR(uint32_t entropy);

extern void SIM_Random_Add(uint32_t entropy);


#ifdef __cplusplus
}
#endif
