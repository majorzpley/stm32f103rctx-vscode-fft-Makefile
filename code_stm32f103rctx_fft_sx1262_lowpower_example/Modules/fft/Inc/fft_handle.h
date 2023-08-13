/***
 * @Author: MajorZpley
 * @Date: 2023-08-03 15:23:42
 * @LastEditTime: 2023-08-03 15:23:43
 * @LastEditors: your name
 * @Description:
 * @FilePath: \stm32f103rctx+sx1262+sound2\Modules\fft\Inc\fft_handle.h
 * @practice makes perfect!
 */
#ifndef FFT_HANDLE_H
#define FFT_HANDLE_H
#include "stdint-gcc.h"
// 通道一采样点数
#define Samp1_Point 1024
// 通道二采样点数
#define Samp2_Point 1024

extern uint16_t ADC1_buff[Samp1_Point];
extern uint16_t ADC2_buff[Samp2_Point];

// GPIO_TypeDef;CRL;CRH;IDR;ODR;BSRR;BRR;LCKR;每个寄存器占用4字节，地址偏移4字节
// GPIOA外设地址:APB2PERIPH_BASE:0x40000000UL+0x00010000UL+0x00000800UL=0x40010800
#define GPIOA_IDR *((unsigned volatile int *)0x40010808) // PA输入寄存器
#define GPIOA_ODR *((unsigned volatile int *)0x4001080C) // PA输出寄存器

// GPIOB外设地址a:0x40010c00
#define GPIOB_IDR *((unsigned volatile int *)0x40010C08) // PA输入寄存器
#define GPIOB_ODR *((unsigned volatile int *)0x40010C0C) // PA输出寄存器

// GPIOC外设地址:0x40011000
#define GPIOC_IDR *((unsigned volatile int *)0x40011008) // PA输入寄存器
#define GPIOC_ODR *((unsigned volatile int *)0x4001100C) // PA输出寄存器

// GPIOD外设地址:0x40011400
#define GPIOD_IDR *((unsigned volatile int *)0x40011408) // PA输入寄存器
#define GPIOD_ODR *((unsigned volatile int *)0x4001140C) // PA输出寄存器

// GPIOE外设地址:0x40011800
#define GPIOE_IDR *((unsigned volatile int *)0x40011808) // PA输入寄存器
#define GPIOE_ODR *((unsigned volatile int *)0x4001180C) // PA输出寄存器

// GPIOF外设地址:0x40011c00
#define GPIOF_IDR *((unsigned volatile int *)0x40011C08) // PA输入寄存器
#define GPIOF_ODR *((unsigned volatile int *)0x40011C0C) // PA输出寄存器

// GPIOG外设地址:0x40012000
#define GPIOG_IDR *((unsigned volatile int *)0x40012008) // PA输入寄存器
#define GPIOG_ODR *((unsigned volatile int *)0x4001200C) // PA输出寄存器

// SAMPLE SCK:PB13
#define SAMPLE_SCK_0 GPIOB_ODR &= 0xDFFF
#define SAMPLE_SCK_1 GPIOB_ODR |= 0X2000

// SAMPLE SDI1:PB14
#define SAMPLE_DI_1 GPIOB_IDR & 0x4000

// SAMPLE SDI2:PB15
#define SAMPLE_DI_2 GPIOB_ODR & 0x8000

// SAMPLE CS:PB12
#define SAMPLE_CS_O GPIOB_ODR &= 0xEFFF
#define SAMPLE_CS_1 GPIOB_ODR |= 0x1000

#define DELAYMS 1
// fn声明

/// @brief fn延时
/// @param n
extern void Delay(unsigned int n);

/// @brief fn模拟读取AD数据
/// @param adc1_data
/// @param adc2_data
extern void ADRead(uint16_t *adc1_data, uint16_t *adc2_data);

/// @brief fft频域一次积分计算频率和振幅
/// @param input 输入
/// @param freq 输出频率
/// @param amplitude 输出幅度
extern void fftGetFreqAmplitude(uint16_t *input, uint16_t samplePoints, float *freq, float *amplitude);
#endif // !FFT_HANDLE_H
