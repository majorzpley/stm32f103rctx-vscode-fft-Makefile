/***
 * @Author: MajorZpley
 * @Date: 2023-08-03 17:51:10
 * @LastEditTime: 2023-08-03 17:59:49
 * @LastEditors: your name
 * @Description: #
 * @FilePath: \stm32f103rctx+sx1262+sound2\Modules\mcu\Inc\mcu_fn.h
 * @practice makes perfect!
 */
#ifndef MCU_FN_H
#define MCU_FN_H
#include "main.h"
#include "rtc.h"
#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"
#include "sx126x_Init.h"
// 设置SN号
#define SN1

#ifdef SN1
#define SN "4923330001"
#endif

#define BYTE1 __attribute__((packed, aligned(1))) // aligned(1)：1字节对齐
typedef struct
{
    uint8_t sn[6];      // SN号
    uint8_t len;        // fraame整个长度
    uint8_t battery;    // 电池电量百分比
    uint16_t frequency; // 单位Hz
    uint16_t amplitude; // 振幅微米,标量
    uint16_t crc16;     // modbuscrc16校验码
} BYTE1 _Frame_Tx;

typedef struct
{
    uint32_t lowSendTime;   // 低于门限值发送时间
    uint32_t highSendTime;  // 高于门限值发送时间
    uint32_t sampleTime;    // 采样时间
    uint32_t ampThreshold;  // 振幅门限值
    uint32_t freqThreshold; // 频率门限值
} _ConfigInfo;
extern _ConfigInfo info;
extern _ConfigInfo debug;

typedef struct
{
    uint32_t freq;          // 频率，RTC_BKP_DR1
    uint32_t ampl;          // 振幅，RTC_BKP_DR2
    uint32_t sampleSec;     // 采样时间，RTC_BKP_DR3
    uint32_t sendSec;       // 发送时间，RTC_BKP_DR4
    uint32_t firstInitiate; // 初次上电，RTC_BKP_DR5
    uint32_t bat;           // 电池电量，RTC_BKP_DR6
    void (*record)(void);   // 记录所有局部变量的值至rtc备份寄存器
    void (*copy)(void);     // 从rtc备份寄存器中获取所有局部变脸的值
} _RTCBKP_STORAGE_VALUE;
extern _RTCBKP_STORAGE_VALUE stackValue;

typedef struct
{
    _Frame_Tx txFrame;
} _MCU_LORA_OBJECT;
extern _MCU_LORA_OBJECT loraObject;
/// @brief mcu进入standby模式
/// @param
extern void mcu_into_standbyMode(void);
/// @brief mcu所有io口进入模拟输入模式
/// @param
extern void mcu_gpio_into_mode_analog(void);
/// @brief CRC16计算
/// @param dat
/// @param lenth
/// @return crc校验码，H|L
extern uint16_t Protocol_Crc16(uint8_t *dat, uint8_t lenth);
/// @brief  生成发送数据帧
/// @param
extern void generateFrame(void);
/// @brief 获取配置信息函数
/// @param timeout
extern uint8_t mcu_wait_get_userinfo(int timeout);
/// @brief 写备份寄存器
/// @param
extern void writeRtcBKP(void);
/// @brief 读备份寄存器
/// @param
extern void readRtcBKP(void);

/// @brief 设置rtc闹钟的唤醒间隔时间
/// @param Seconds 表示多少秒后唤醒
void mcu_rtc_alarmSet(uint32_t Seconds);
#endif
