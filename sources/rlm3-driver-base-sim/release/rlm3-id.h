#pragma once

#include "rlm3-base.h"


#ifdef __cplusplus
extern "C" {
#endif


#define RLM3_ID_SIZE (12)

extern uint8_t RLM3_ID_Get(size_t index);

extern void SIM_ID_Set(size_t index, uint8_t value);


#ifdef __cplusplus
}
#endif
