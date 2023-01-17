#pragma once

#include "rlm3-base.h"
#include "rlm3-task.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
	volatile bool is_locked;
} RLM3_SpinLock;

typedef struct
{
	volatile bool is_locked;
} RLM3_MutexLock;

extern void RLM3_Lock_EnterCritical();
extern void RLM3_Lock_ExitCritical();
extern uint32_t RLM3_Lock_EnterCriticalISR();
extern void RLM3_Lock_ExitCriticalISR(uint32_t saved_level);

extern void RLM3_SpinLock_Init(RLM3_SpinLock* lock);
extern void RLM3_SpinLock_Deinit(RLM3_SpinLock* lock);
extern void RLM3_SpinLock_Enter(RLM3_SpinLock* lock);
extern bool RLM3_SpinLock_Try(RLM3_SpinLock* lock, size_t timeout_ms);
extern void RLM3_SpinLock_Leave(RLM3_SpinLock* lock);

extern void RLM3_MutexLock_Init(RLM3_MutexLock* lock);
extern void RLM3_MutexLock_Deinit(RLM3_MutexLock* lock);
extern void RLM3_MutexLock_Enter(RLM3_MutexLock* lock);
extern bool RLM3_MutexLock_Try(RLM3_MutexLock* lock, size_t timeout_ms);
extern void RLM3_MutexLock_Leave(RLM3_MutexLock* lock);

extern bool SIM_IsInCritical();


#ifdef __cplusplus
}
#endif
