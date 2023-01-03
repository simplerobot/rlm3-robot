#pragma once

#include "rlm3-base.h"

extern void CommOutput_Init();
extern void CommOutput_RunTask();
extern void CommOutput_AbortTask();
extern void CommOutput_AbortTaskISR();

extern void CommOutput_OpenConnectionISR(size_t link_id);
extern void CommOutput_CloseConnectionISR(size_t link_id);
