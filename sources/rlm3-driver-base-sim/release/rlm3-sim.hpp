#pragma once

#include "rlm3-base.h"
#include "rlm3-task.h"

#ifndef __cplusplus
#error Sim definitions use C++ features.
#endif

#ifndef TEST
#error Sim definitions only work in test environments.
#endif

#include <functional>
#include <string>


extern bool SIM_IsISR();
extern bool SIM_IsMainTask();

extern void SIM_AddInterrupt(std::function<void()> interrupt);
extern void SIM_AddTask(std::function<void()> task);
extern void SIM_AddDelay(RLM3_Time delay);
extern void SIM_DoInterrupt(std::function<void()> interrupt);
extern void SIM_DoTask(std::function<void()> task);

extern bool SIM_HasNextHandler();
extern RLM3_Time SIM_GetNextHandlerTime();
extern void SIM_RunNextHandler();

extern std::string SIM_SafeString(const std::string& input);


