/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
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
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
