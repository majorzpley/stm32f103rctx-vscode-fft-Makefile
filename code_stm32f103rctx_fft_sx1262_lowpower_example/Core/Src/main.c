/*
 * @Author: MajorZpley
 * @Date: 2023-08-03 11:24:38
 * @LastEditTime: 2023-08-04 20:21:51
 * @LastEditors: your name
 * @Description:
 * @FilePath: \stm32f103rctx+sx1262+sound2\Core\Src\main.c
 * practice makes perfect!
 */
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "adc.h"
#include "rtc.h"
#include "radio.h"
#include "sx126x.h"
#include "sx126x_Init.h"
#include "sx126x-board.h"
#include "fft_handle.h"
#include "double_integral.h"
#include "WindowCoeff.h"
#include "mcu_flash.h"
#include "mcu_fn.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t bat = 0; // 电池电量
float freq = 0.0f;
float amplitude = 0.0f;
uint8_t signalsend = 0;
#ifdef SN1
uint8_t rf_tx[10] = {0xBB, 0x49, 0x23, 0x33, 0x00, 0x01, 0x00, 0x64}; // lora返回的配置成功响应数据
#endif
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  // TODO:原始数据调试串口
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  MX_ADC1_Init();
  Radio_Init();
  Init_WindowCoef1024();
  // FlashWrite(FLASH_SAVE_ADDR, (uint16_t *)&debug, sizeof(_ConfigInfo));
  FlashRead(FLASH_SAVE_ADDR, (uint16_t *)&info, sizeof(_ConfigInfo));
  MX_RTC_Init();
  stackValue.copy();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    LED_ON;
    HAL_Delay(50);
    if ((!stackValue.firstInitiate) || (stackValue.sampleSec >= info.sampleTime))
    {
      EN_5V_ENABLE;
      HAL_Delay(100);
      stackValue.bat = get_voltage(); // 更新电池电量
      for (size_t j = 0; j < Samp1_Point; j++)
      {
        ADRead(&ADC1_buff[j], &ADC2_buff[j]);
      }
      // EN_5V_DISABLE;
      fftGetFreqAmplitude(ADC1_buff, Samp1_Point, &freq, &amplitude);
      // TAG:打印调试信息
      HAL_UART_Transmit(&huart3, (uint8_t *)ADC1_buff, Samp1_Point * 2, 0xffffff);
      HAL_Delay(5);
      HAL_UART_Transmit(&huart3, (uint8_t *)ADC2_buff, Samp2_Point * 2, 0xffffff);
      // BUG:需要重新对频率进行分段计算
      // freq *= 1.3;
      stackValue.sampleSec = 0;
      stackValue.ampl = (uint32_t)amplitude;
      stackValue.freq = (uint32_t)freq;
    }
    if (!stackValue.firstInitiate)
    {
      signalsend = 1;
    }
    if (stackValue.freq > info.freqThreshold || stackValue.ampl > info.ampThreshold)
    {
      if (stackValue.sendSec >= info.highSendTime)
      {
        signalsend = 1;
      }
    }
    else
    {
      if (stackValue.sendSec >= info.lowSendTime)
      {
        signalsend = 1;
      }
    }
    // if (signalsend)
    // {
    //   generateFrame();
    //   Radio.Standby();
    //   Radio.SetChannel(LORA_TX_FRE);
    //   Radio_Send((uint8_t *)&loraObject.txFrame, sizeof(loraObject.txFrame), 1000);
    //   if (mcu_wait_get_userinfo(3000))
    //   {
    //     /*解析成功回复*/
    //     uint16_t crc16 = Protocol_Crc16(rf_tx, 8);
    //     rf_tx[8] = crc16 & 0xff;
    //     rf_tx[9] = crc16 >> 8;
    //     Radio.SetChannel(LORA_TX_FRE); // 切换成发送频率
    //     Radio.Standby();
    //     Radio_Send(rf_tx, 10, 1000);
    //   }
    //   stackValue.sendSec = 0;
    //   signalsend = 0;
    // }
    // Radio.Sleep();
    // EN_5V_DISABLE;
    // LED_OFF;
    // stackValue.record();
    // mcu_gpio_into_mode_analog();
    // mcu_rtc_alarmSet(9);
    // mcu_into_standbyMode();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_ADC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
