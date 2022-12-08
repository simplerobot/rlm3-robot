#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


#define RLM3_FLASH_SIZE (2048)

extern void RLM3_Flash_Init();
extern void RLM3_Flash_Deinit();
extern bool RLM3_Flash_IsInit();

extern bool RLM3_Flash_Write(uint32_t flash_address, const uint8_t* data, size_t size);
extern bool RLM3_Flash_Read(uint32_t flash_address, uint8_t* data, size_t size);

extern void SIM_Flash_Write(uint32_t flash_address, uint8_t data);
extern uint8_t SIM_Flash_Read(uint32_t flash_address);
extern void SIM_Flash_ForceFailure();


#ifdef __cplusplus
}
#endif
