/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

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
#define RF_NIRQ_Pin GPIO_PIN_3
#define RF_NIRQ_GPIO_Port GPIOC
#define SPI1_CS_Pin GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define RF_RST_Pin GPIO_PIN_4
#define RF_RST_GPIO_Port GPIOC
#define RF_BUSY_Pin GPIO_PIN_5
#define RF_BUSY_GPIO_Port GPIOC
#define EN_5V_Pin GPIO_PIN_2
#define EN_5V_GPIO_Port GPIOB
#define DEBUG_TX_Pin GPIO_PIN_10
#define DEBUG_TX_GPIO_Port GPIOB
#define DEBUG_RX_Pin GPIO_PIN_11
#define DEBUG_RX_GPIO_Port GPIOB
#define SAMPLE_CS_Pin GPIO_PIN_12
#define SAMPLE_CS_GPIO_Port GPIOB
#define SAMPLE_SCK_Pin GPIO_PIN_13
#define SAMPLE_SCK_GPIO_Port GPIOB
#define SAMPLE_NO1_Pin GPIO_PIN_14
#define SAMPLE_NO1_GPIO_Port GPIOB
#define SAMPLE_NO2_Pin GPIO_PIN_15
#define SAMPLE_NO2_GPIO_Port GPIOB
#define WDOG_Pin GPIO_PIN_6
#define WDOG_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_7
#define LED_GPIO_Port GPIOB
#define BATT_DET_GPIO_Port GPIOB
#define BATT_DET_Pin GPIO_PIN_0

/* USER CODE BEGIN Private defines */
// LED状态
#define LED_ON HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET)
#define LED_OFF HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET)
// 5V使能状态
#define EN_5V_ENABLE HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, GPIO_PIN_SET)
#define EN_5V_DISABLE HAL_GPIO_WritePin(EN_5V_GPIO_Port, EN_5V_Pin, GPIO_PIN_RESET)
// RFspi1 cs引脚状态
#define RF_CS_L HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_RESET)
#define RF_CS_H HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET)
// RF NIRQ引脚状态
#define IS_RF_NIRQ HAL_GPIO_ReadPin(RF_NIRQ_GPIO_Port, RF_NIRQ_Pin)
// RF RST引脚
#define RF_RST_L HAL_GPIO_WritePin(RF_RST_GPIO_Port, RF_RST_Pin, GPIO_PIN_RESET)
#define RF_RST_H HAL_GPIO_WritePin(RF_RST_GPIO_Port, RF_RST_Pin, GPIO_PIN_SET)
// RF BUSY
#define IS_RF_BUSY HAL_GPIO_ReadPin(RF_BUSY_GPIO_Port, RF_BUSY_Pin)
// wdog feed
#define WDOG_FEED HAL_GPIO_TogglePin(WDOG_GPIO_Port, WDOG_Pin)
  /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
