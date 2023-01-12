#pragma once

#include "rlm3-base.h"


#ifdef __cplusplus
extern "C" {
#endif


extern void RLM3_Motors_Init();
extern void RLM3_Motors_DeInit();
extern bool RLM3_Motors_IsInit();
extern void RLM3_Motors_Enable();
extern void RLM3_Motors_Disable();
extern bool RLM3_Motors_IsEnabled();
extern void RLM3_Motors_SetWheels(int8_t left, int8_t right);
extern void RLM3_Motors_SetBlade(int8_t blade);


#ifdef __cplusplus
}
#endif
