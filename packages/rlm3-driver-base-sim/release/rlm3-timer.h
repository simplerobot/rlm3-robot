#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


extern void RLM3_Timer2_Init(size_t frequency_hz);
extern void RLM3_Timer2_Deinit();
extern bool RLM3_Timer2_IsInit();
extern void RLM3_Timer2_Event_Callback();

extern void SIM_Timer2_Event();


#ifdef __cplusplus
}
#endif


