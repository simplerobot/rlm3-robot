#pragma once

#include "rlm3-base.h"


#ifdef __cplusplus
extern "C" {
#endif


extern bool RLM3_Debug_IsActive();
extern bool RLM3_Debug_IsActiveISR();

extern bool RLM3_Debug_Output(uint8_t c);
extern bool RLM3_Debug_OutputISR(uint8_t c);
extern bool RLM3_Debug_OutputSafe(uint8_t c);


#ifdef __cplusplus
}
#endif
