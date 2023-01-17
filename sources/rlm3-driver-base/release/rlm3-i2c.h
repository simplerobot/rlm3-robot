#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	RLM3_I2C_DEVICE_TEST,
	RLM3_I2C_DEVICE_CAMERA,
	RLM3_I2C_DEVICE_MAGNETIC,
	RLM3_I2C_DEVICE_FLASH,
	RLM3_I2C_DEVICE_COUNT
} RLM3_I2C_DEVICE;


extern void RLM3_I2C_Init(RLM3_I2C_DEVICE device);
extern void RLM3_I2C_Deinit(RLM3_I2C_DEVICE device);
extern bool RLM3_I2C_IsInit(RLM3_I2C_DEVICE device);

extern bool RLM3_I2C_Transmit(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* data, size_t size);
extern bool RLM3_I2C_Receive(RLM3_I2C_DEVICE device, uint32_t addr, uint8_t* data, size_t size);
extern bool RLM3_I2C_TransmitReceive(RLM3_I2C_DEVICE device, uint32_t addr, const uint8_t* tx_data, size_t tx_size, uint8_t* rx_data, size_t rx_size);


#ifdef __cplusplus
}
#endif
