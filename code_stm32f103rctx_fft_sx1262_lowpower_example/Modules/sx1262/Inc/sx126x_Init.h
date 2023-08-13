/***
 * @Author: MajorZpley
 * @Date: 2023-08-03 16:45:45
 * @LastEditTime: 2023-08-04 15:35:59
 * @LastEditors: your name
 * @Description:
 * @FilePath: \stm32f103rctx+sx1262+sound2\Modules\sx1262\Inc\sx126x_Init.h
 * @practice makes perfect!
 */
#ifndef __SX126X_INIT_H__
#define __SX126X_INIT_H__

#include "stdint-gcc.h"
// 基于semtech官网驱动移植
// 官网驱动网址 https://github.com/Lora-net/LoRaMac-node/tree/master/src/radio  下载日期 2021/2/3
#define SOFT_VERSION "sx126x driver for stm32f103 V0.0.0"
#define _SN1
//--------------------------------------------- 测试默认配置 ---------------------------------------------

#define LORA_RX_FRE 520000000 // 接收频率

#ifdef _SN1
#define LORA_TX_FRE 471000000 // 发送频率

#elif defined _SN2
#define LORA_TX_FRE 473000000 // 发送频率

#elif defined _SN3
#define LORA_TX_FRE 475000000 // 发送频率

#elif defined _SN4
#define LORA_TX_FRE 477000000 // 发送频率

#endif

// #define LORA_RX_FRE																	466000000	// 接收频率
// #define LORA_TX_FRE																	475500000	// 发送频率
#define LORA_TX_OUTPUT_POWER 20			 // 测试默认使用的发射功率，126x发射功率0~22dbm，127x发射功率2~20dbm
#define LORA_BANDWIDTH 2				 // [0: 125 kHz,	测试默认使用的带宽，sx126x：[0: 125 kHz,1: 250 kHz,2: 500 kHz,3: Reserved]
#define LORA_SPREADING_FACTOR 7			 // 测试默认使用的扩频因子范围7~12
#define LORA_CODINGRATE 1				 // 测试默认使用的纠错编码率[1: 4/5,2: 4/6,3: 4/7,4: 4/8]
#define LORA_PREAMBLE_LENGTH 8			 // 前导码长度
#define LORA_SX126X_SYMBOL_TIMEOUT 0	 // Symbols(sx126x用到的是0,127x用到的是5)
#define LORA_FIX_LENGTH_PAYLOAD_ON false // 是否为固定长度包(暂时只是sx126x用到了)
#define LORA_IQ_INVERSION_ON false		 // 这个应该是设置是否翻转中断电平的(暂时只是sx126x用到了)
#define LORA_RX_TIMEOUT_VALUE 0

#define TX_OUTPUT_POWER 20		   // dBm
#define FSK_FDEV 25e3			   // Hz
#define FSK_DATARATE 1200		   // bps
#define FSK_BANDWIDTH 22e3		   // Hz
#define FSK_AFC_BANDWIDTH 83.333e3 // Hz
#define FSK_PREAMBLE_LENGTH 5	   // Same for Tx and Rx
#define FSK_FIX_LENGTH_PAYLOAD_ON false

typedef enum
{
	LOWPOWER,
	RX_Done,
	RX_TIMEOUT,
	RX_ERROR,
	TX_Done,
	TX_TIMEOUT,
	Null
} States_t;
extern States_t SX1262_State;
extern unsigned char RF_RX_Buffer[256];
/*!
 * Board MCU pins definitions
 */
// SPI
void Radio_Init(void);
void Radio_Init_RXD(void);
void SX126xDelayMs(void);
void SX126xWaitOnBusy(void);
void SX126xReset(void);
void Radio_Send(uint8_t *pData, unsigned char len, uint16_t time_out);
void Radio_Recev(uint16_t time_out);
void Radio_IrqProcess(void);
void Radio_startCad(uint16_t count);
#endif
