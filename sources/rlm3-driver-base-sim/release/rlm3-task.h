#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct opaque_RLM3_Task_Ref* RLM3_Task;
typedef uint32_t RLM3_Time;
typedef void (*RLM3_Task_Fn)();

extern RLM3_Time RLM3_Time_Get();
extern RLM3_Time RLM3_Time_GetISR();

extern bool RLM3_Task_IsSchedulerRunning();
extern bool RLM3_Task_IsISR();

extern RLM3_Task RLM3_Task_Create(RLM3_Task_Fn fn, size_t stack_size_words, const char* name);

extern RLM3_Task RLM3_Task_GetCurrent();
extern void RLM3_Task_Yield();
extern void RLM3_Task_Delay(RLM3_Time time_ms);
extern void RLM3_Task_DelayUntil(RLM3_Time start_time, RLM3_Time delay_ms);

extern void RLM3_Task_Give(RLM3_Task task);
extern void RLM3_Task_GiveISR(RLM3_Task task);
extern void RLM3_Task_Take();
extern bool RLM3_Task_TakeWithTimeout(RLM3_Time timeout_ms);
extern bool RLM3_Task_TakeUntil(RLM3_Time start_time, RLM3_Time delay_ms);

extern void SIM_Give();


#ifdef __cplusplus
}
#endif


