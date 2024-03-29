#pragma once

#include "rlm3-base.h"

extern void CommInput_RunTask();
extern void CommInput_AbortTask();
extern void CommInput_AbortTaskISR();

extern bool CommInput_PutMessageByteISR(uint8_t byte);
extern void CommInput_ResetPipeISR();


extern void CommInput_Control_Callback(uint32_t time, float left, float right, float blade);
