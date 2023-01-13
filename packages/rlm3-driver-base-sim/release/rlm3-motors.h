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
extern void RLM3_Motors_SetWheels(float left, float right);
extern void RLM3_Motors_SetBlade(float blade);

extern float SIM_Motors_GetLeft();
extern float SIM_Motors_GetRight();
extern float SIM_Motors_GetBlade();


#ifdef __cplusplus
}
#endif
