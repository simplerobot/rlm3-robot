#pragma once

#include "rlm3-base.h"


#ifdef __cplusplus
extern "C" {
#endif


extern void RLM3_GPIO_Init();
extern void RLM3_GPIO_DeInit();
extern bool RLM3_GPIO_IsInit();

extern void RLM3_GPIO_CameraPowerDown_SetHigh();
extern void RLM3_GPIO_CameraPowerDown_SetLow();
extern bool RLM3_GPIO_CameraPowerDown_IsHigh();
extern bool RLM3_GPIO_CameraPowerDown_IsLow();
extern void RLM3_GPIO_CameraReset_SetHigh();
extern void RLM3_GPIO_CameraReset_SetLow();
extern bool RLM3_GPIO_CameraReset_IsHigh();
extern bool RLM3_GPIO_CameraReset_IsLow();
extern void RLM3_GPIO_GpsReset_SetHigh();
extern void RLM3_GPIO_GpsReset_SetLow();
extern bool RLM3_GPIO_GpsReset_IsHigh();
extern bool RLM3_GPIO_GpsReset_IsLow();
extern void RLM3_GPIO_InertiaChipSelect_SetHigh();
extern void RLM3_GPIO_InertiaChipSelect_SetLow();
extern bool RLM3_GPIO_InertiaChipSelect_IsHigh();
extern bool RLM3_GPIO_InertiaChipSelect_IsLow();
extern void RLM3_GPIO_MotorEnable_SetHigh();
extern void RLM3_GPIO_MotorEnable_SetLow();
extern bool RLM3_GPIO_MotorEnable_IsHigh();
extern bool RLM3_GPIO_MotorEnable_IsLow();
extern void RLM3_GPIO_PowerEnable_SetHigh();
extern void RLM3_GPIO_PowerEnable_SetLow();
extern bool RLM3_GPIO_PowerEnable_IsHigh();
extern bool RLM3_GPIO_PowerEnable_IsLow();
extern void RLM3_GPIO_StatusLight_SetHigh();
extern void RLM3_GPIO_StatusLight_SetLow();
extern bool RLM3_GPIO_StatusLight_IsHigh();
extern bool RLM3_GPIO_StatusLight_IsLow();
extern void RLM3_GPIO_WifiEnable_SetHigh();
extern void RLM3_GPIO_WifiEnable_SetLow();
extern bool RLM3_GPIO_WifiEnable_IsHigh();
extern bool RLM3_GPIO_WifiEnable_IsLow();
extern void RLM3_GPIO_WifiBootMode_SetHigh();
extern void RLM3_GPIO_WifiBootMode_SetLow();
extern bool RLM3_GPIO_WifiBootMode_IsHigh();
extern bool RLM3_GPIO_WifiBootMode_IsLow();
extern bool RLM3_GPIO_BaseSense_IsHigh();
extern bool RLM3_GPIO_BaseSense_IsLow();
extern bool RLM3_GPIO_GpsPulse_IsHigh();
extern bool RLM3_GPIO_GpsPulse_IsLow();
extern bool RLM3_GPIO_StopA_IsHigh();
extern bool RLM3_GPIO_StopA_IsLow();
extern bool RLM3_GPIO_StopB_IsHigh();
extern bool RLM3_GPIO_StopB_IsLow();

extern void RLM3_GPIO_BaseSense_Event_CB_ISR();
extern void RLM3_GPIO_GpsPulse_Event_CB_ISR();
extern void RLM3_GPIO_StopA_Event_CB_ISR();
extern void RLM3_GPIO_StopB_Event_CB_ISR();


#ifdef __cplusplus
}
#endif
