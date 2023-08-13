#ifndef __MCU_FLASH_H
#define __MCU_FLASH_H
#include "main.h"
#include "stm32f1xx_hal.h"
#define FLASH_SAVE_ADDR 0X08020000
#define STM32_FLASH_BASE 0x08000000 // STM32 FLASH的起始地址
#define STM32_FLASH_SIZE 256
#define FLASH_WAITETIME 50000 // FLASH等待超时时间
extern void FlashRead(uint32_t ReadAddr, uint16_t *pBuffer, uint16_t NumToRead);
extern void FlashWrite(uint32_t WriteAddr, uint16_t *pBuffer, uint16_t NumToWrite);
#endif // !__MCU_FLASH_H