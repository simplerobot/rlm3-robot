#pragma once

#include "rlm3-base.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum
{
	GPIOA,
	GPIOB,
	GPIOC,
	GPIOD,
	GPIOE,
	GPIOF,
	GPIOG,
	GPIOH,
	GPIOI,
	GPIOJ,
	GPIOK,
	GPIO_COUNT
} GPIO_Port;


#define GPIO_PIN_0  ((uint16_t)0x0001)
#define GPIO_PIN_1  ((uint16_t)0x0002)
#define GPIO_PIN_2  ((uint16_t)0x0004)
#define GPIO_PIN_3  ((uint16_t)0x0008)
#define GPIO_PIN_4  ((uint16_t)0x0010)
#define GPIO_PIN_5  ((uint16_t)0x0020)
#define GPIO_PIN_6  ((uint16_t)0x0040)
#define GPIO_PIN_7  ((uint16_t)0x0080)
#define GPIO_PIN_8  ((uint16_t)0x0100)
#define GPIO_PIN_9  ((uint16_t)0x0200)
#define GPIO_PIN_10 ((uint16_t)0x0400)
#define GPIO_PIN_11 ((uint16_t)0x0800)
#define GPIO_PIN_12 ((uint16_t)0x1000)
#define GPIO_PIN_13 ((uint16_t)0x2000)
#define GPIO_PIN_14 ((uint16_t)0x4000)
#define GPIO_PIN_15 ((uint16_t)0x8000)


#define CAMERA_POWER_DOWN_Pin GPIO_PIN_3
#define CAMERA_POWER_DOWN_GPIO_Port GPIOE
#define STOP_B_Pin GPIO_PIN_13
#define STOP_B_GPIO_Port GPIOC
#define VOLTAGE_SENSOR_Pin GPIO_PIN_7
#define VOLTAGE_SENSOR_GPIO_Port GPIOF
#define BLADE_MOTOR_Pin GPIO_PIN_8
#define BLADE_MOTOR_GPIO_Port GPIOF
#define BLADE_BRAKE_Pin GPIO_PIN_9
#define BLADE_BRAKE_GPIO_Port GPIOF
#define BATTERY_TEMP_Pin GPIO_PIN_10
#define BATTERY_TEMP_GPIO_Port GPIOF
#define LEFT_SENSOR_Pin GPIO_PIN_1
#define LEFT_SENSOR_GPIO_Port GPIOC
#define BASE_SENSE_Pin GPIO_PIN_2
#define BASE_SENSE_GPIO_Port GPIOC
#define RIGHT_SENSOR_Pin GPIO_PIN_3
#define RIGHT_SENSOR_GPIO_Port GPIOC
#define WIFI_TX_Pin GPIO_PIN_0
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_1
#define WIFI_RX_GPIO_Port GPIOA
#define GPS_TX_Pin GPIO_PIN_2
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin GPIO_PIN_3
#define GPS_RX_GPIO_Port GPIOA
#define CAMERA_LIGHT_Pin GPIO_PIN_7
#define CAMERA_LIGHT_GPIO_Port GPIOA
#define BLADE_SENSOR_Pin GPIO_PIN_4
#define BLADE_SENSOR_GPIO_Port GPIOC
#define CURRENT_SENSOR_Pin GPIO_PIN_5
#define CURRENT_SENSOR_GPIO_Port GPIOC
#define LEFT_BACKWARD_Pin GPIO_PIN_0
#define LEFT_BACKWARD_GPIO_Port GPIOB
#define LEFT_FORWARD_Pin GPIO_PIN_1
#define LEFT_FORWARD_GPIO_Port GPIOB
#define TEST_TX_Pin GPIO_PIN_10
#define TEST_TX_GPIO_Port GPIOB
#define TEST_RX_Pin GPIO_PIN_11
#define TEST_RX_GPIO_Port GPIOB
#define GPS_RESET_Pin GPIO_PIN_12
#define GPS_RESET_GPIO_Port GPIOB
#define CAMERA_RESET_Pin GPIO_PIN_11
#define CAMERA_RESET_GPIO_Port GPIOD
#define CHARGER_Pin GPIO_PIN_12
#define CHARGER_GPIO_Port GPIOD
#define POWER_ENABLE_Pin GPIO_PIN_2
#define POWER_ENABLE_GPIO_Port GPIOG
#define MOTOR_ENABLE_Pin GPIO_PIN_7
#define MOTOR_ENABLE_GPIO_Port GPIOG
#define CAMERA_CLOCK_Pin GPIO_PIN_8
#define CAMERA_CLOCK_GPIO_Port GPIOA
#define RIGHT_FORWARD_Pin GPIO_PIN_9
#define RIGHT_FORWARD_GPIO_Port GPIOA
#define RIGHT_BACKWARD_Pin GPIO_PIN_10
#define RIGHT_BACKWARD_GPIO_Port GPIOA
#define STOP_A_Pin GPIO_PIN_11
#define STOP_A_GPIO_Port GPIOC
#define INERTIA_CHIP_SELECT_Pin GPIO_PIN_4
#define INERTIA_CHIP_SELECT_GPIO_Port GPIOD
#define STATUS_LIGHT_Pin GPIO_PIN_5
#define STATUS_LIGHT_GPIO_Port GPIOD
#define WIFI_ENABLE_Pin GPIO_PIN_10
#define WIFI_ENABLE_GPIO_Port GPIOG
#define WIFI_BOOT_MODE_Pin GPIO_PIN_11
#define WIFI_BOOT_MODE_GPIO_Port GPIOG
#define GPS_PULSE_Pin GPIO_PIN_12
#define GPS_PULSE_GPIO_Port GPIOG
#define WIFI_RESET_Pin GPIO_PIN_13
#define WIFI_RESET_GPIO_Port GPIOG


typedef enum
{
	GPIO_MODE_DISABLED,
	GPIO_MODE_INPUT,
	GPIO_MODE_OUTPUT_PP,
	GPIO_MODE_OUTPUT_OD,
	GPIO_MODE_AF_PP,
	GPIO_MODE_AF_OD,
	GPIO_MODE_ANALOG,
	GPIO_MODE_IT_RISING,
	GPIO_MODE_IT_FALLING,
	GPIO_MODE_IT_RISING_FALLING,
	GPIO_MODE_EVT_RISING,
	GPIO_MODE_EVT_FALLING,
	GPIO_MODE_EVT_RISING_FALLING,
} GPIO_Mode;

typedef enum
{
	GPIO_PULL_DISABLED,
	GPIO_NOPULL,
	GPIO_PULLUP,
	GPIO_PULLDOWN,
} GPIO_Pull;

typedef enum
{
	GPIO_SPEED_DISABLED,
	GPIO_SPEED_FREQ_LOW,
	GPIO_SPEED_FREQ_MEDIUM,
	GPIO_SPEED_FREQ_HIGH,
	GPIO_SPEED_FREQ_VERY_HIGH,
} GPIO_Speed;

typedef enum
{
	GPIO_AF_DISABLED,
	GPIO_AF0_RTC_50Hz,
	GPIO_AF0_MCO,
	GPIO_AF0_TAMPER,
	GPIO_AF0_SWJ,
	GPIO_AF0_TRACE,
	GPIO_AF1_TIM1,
	GPIO_AF1_TIM2,
	GPIO_AF2_TIM3,
	GPIO_AF2_TIM4,
	GPIO_AF2_TIM5,
	GPIO_AF3_TIM8,
	GPIO_AF3_TIM9,
	GPIO_AF3_TIM10,
	GPIO_AF3_TIM11,
	GPIO_AF4_I2C1,
	GPIO_AF4_I2C2,
	GPIO_AF4_I2C3,
	GPIO_AF5_SPI1,
	GPIO_AF5_SPI2,
	GPIO_AF5_SPI3,
	GPIO_AF5_SPI4,
	GPIO_AF5_SPI5,
	GPIO_AF5_SPI6,
	GPIO_AF6_SPI3,
	GPIO_AF6_SAI1,
	GPIO_AF7_USART1,
	GPIO_AF7_USART2,
	GPIO_AF7_USART3,
	GPIO_AF8_UART4,
	GPIO_AF8_UART5,
	GPIO_AF8_USART6,
	GPIO_AF8_UART7,
	GPIO_AF8_UART8,
	GPIO_AF9_CAN1,
	GPIO_AF9_CAN2,
	GPIO_AF9_TIM12,
	GPIO_AF9_TIM13,
	GPIO_AF9_TIM14,
	GPIO_AF10_OTG_FS,
	GPIO_AF10_OTG_HS,
	GPIO_AF11_ETH,
	GPIO_AF12_FMC,
	GPIO_AF12_OTG_HS_FS,
	GPIO_AF12_SDIO,
	GPIO_AF13_DCMI,
	GPIO_AF15_EVENTOUT,
} GPIO_AlternateFunction;

typedef struct
{
	uint32_t Pin;
	GPIO_Mode Mode;
	GPIO_Pull Pull;
	GPIO_Speed Speed;
	GPIO_AlternateFunction Alternate;
} GPIO_InitTypeDef;

typedef enum
{
	GPIO_PIN_RESET,
	GPIO_PIN_SET,
} GPIO_PinState;

extern void HAL_GPIO_Init(GPIO_Port port, GPIO_InitTypeDef* init);
extern void HAL_GPIO_DeInit(GPIO_Port port, uint32_t pin);
extern GPIO_PinState HAL_GPIO_ReadPin(GPIO_Port port, uint32_t pin);
extern void HAL_GPIO_WritePin(GPIO_Port port, uint32_t pin, GPIO_PinState state);

extern void GPIO_CLOCK_ENABLE(GPIO_Port port);
extern void GPIO_CLOCK_DISABLE(GPIO_Port port);

#define __HAL_RCC_GPIOA_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOA)
#define __HAL_RCC_GPIOB_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOB)
#define __HAL_RCC_GPIOC_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOC)
#define __HAL_RCC_GPIOD_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOD)
#define __HAL_RCC_GPIOE_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOE)
#define __HAL_RCC_GPIOF_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOF)
#define __HAL_RCC_GPIOG_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOG)
#define __HAL_RCC_GPIOH_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOH)
#define __HAL_RCC_GPIOI_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOI)
#define __HAL_RCC_GPIOJ_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOJ)
#define __HAL_RCC_GPIOK_CLK_ENABLE() GPIO_CLOCK_ENABLE(GPIOK)

#define __HAL_RCC_GPIOA_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOA)
#define __HAL_RCC_GPIOB_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOB)
#define __HAL_RCC_GPIOC_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOC)
#define __HAL_RCC_GPIOD_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOD)
#define __HAL_RCC_GPIOE_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOE)
#define __HAL_RCC_GPIOF_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOF)
#define __HAL_RCC_GPIOG_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOG)
#define __HAL_RCC_GPIOH_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOH)
#define __HAL_RCC_GPIOI_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOI)
#define __HAL_RCC_GPIOJ_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOJ)
#define __HAL_RCC_GPIOK_CLK_DISABLE() GPIO_CLOCK_DISABLE(GPIOK)


extern void RLM3_EXTI12_Callback();


extern bool SIM_GPIO_IsClockEnabled(GPIO_Port port);
extern bool SIM_GPIO_IsEnabled(GPIO_Port port, uint32_t pin);
extern GPIO_Mode SIM_GPIO_GetMode(GPIO_Port port, uint32_t pin);
extern GPIO_Pull SIM_GPIO_GetPull(GPIO_Port port, uint32_t pin);
extern GPIO_Speed SIM_GPIO_GetSpeed(GPIO_Port port, uint32_t pin);
extern GPIO_AlternateFunction SIM_GPIO_GetAlt(GPIO_Port port, uint32_t pin);
extern void SIM_GPIO_Write(GPIO_Port port, uint32_t pin, bool value);
extern bool SIM_GPIO_Read(GPIO_Port port, uint32_t pin);
extern void SIM_GPIO_Interrupt(GPIO_Port port, uint32_t pin);


#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
	#include <iostream>

	const char* ToString(GPIO_Port port);
	const char* ToString(GPIO_Mode mode);
	const char* ToString(GPIO_Pull pull);
	const char* ToString(GPIO_Speed speed);
	const char* ToString(GPIO_AlternateFunction alternate);
	const char* ToString(GPIO_PinState state);

	std::ostream& operator<<(std::ostream& out, GPIO_Port port);
	std::ostream& operator<<(std::ostream& out, GPIO_Mode mode);
	std::ostream& operator<<(std::ostream& out, GPIO_Pull pull);
	std::ostream& operator<<(std::ostream& out, GPIO_Speed speed);
	std::ostream& operator<<(std::ostream& out, GPIO_AlternateFunction alternate);
	std::ostream& operator<<(std::ostream& out, GPIO_PinState state);

#endif
