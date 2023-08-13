/*
 * @Author: MajorZpley
 * @Date: 2023-08-03 17:51:01
 * @LastEditTime: 2023-08-03 18:20:05
 * @LastEditors: your name
 * @Description:
 * @FilePath: \stm32f103rctx+sx1262+sound2\Modules\mcu\Src\mcu_fn.c
 * practice makes perfect!
 */
#include "mcu_fn.h"
#include "string.h"
#include "stdio.h"
#include "mcu_flash.h"
// 初始化lora对象
_MCU_LORA_OBJECT loraObject = {
    .txFrame = {0},
};

_ConfigInfo info = {
    .ampThreshold = 100,  // 100微米
    .freqThreshold = 100, // 100Hz
    .highSendTime = 60,   // 60S
    .lowSendTime = 120,   // 120S
    .sampleTime = 10,     // 10S
};

_ConfigInfo debug = {
    .ampThreshold = 10000,  // 100微米
    .freqThreshold = 10000, // 100Hz
    .highSendTime = 60,     // 60S
    .lowSendTime = 120,     // 120S
    .sampleTime = 10,       // 10S
};

_RTCBKP_STORAGE_VALUE stackValue = {
    .ampl = 0,
    .freq = 0,
    .sampleSec = 0,
    .sendSec = 0,
    .record = writeRtcBKP,
    .copy = readRtcBKP,
};

void mcu_into_standbyMode(void)
{
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    HAL_SuspendTick();
    HAL_PWR_EnterSTANDBYMode();
}

void mcu_gpio_into_mode_analog(void)
{
    GPIO_InitTypeDef gpioStructure;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpioStructure.Pin = GPIO_PIN_All;
    gpioStructure.Mode = GPIO_MODE_ANALOG;
    gpioStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &gpioStructure);
    HAL_GPIO_Init(GPIOB, &gpioStructure);
    HAL_GPIO_Init(GPIOC, &gpioStructure);
    HAL_GPIO_Init(GPIOD, &gpioStructure);
}

/// @brief 将16进制的数组转换成字符串，如0x12345678->"12345678"
/// @param Buff 输入的数组
/// @param BuffLen 输入的数组长度
/// @param OutputStr 输出的字符串
/// @return 字符串字节数
static int ArrayToStr(unsigned char *Buff, unsigned int BuffLen, char *OutputStr)
{
    int i = 0;
    char TempBuff[128] = {0};
    char strBuff[256] = {0};

    for (i = 0; i < BuffLen; i++)
    {
        sprintf(TempBuff, "%02x", Buff[i]);      // 以十六进制格式输出到TempBuff，宽度为2
        strncat(strBuff, TempBuff, BuffLen * 2); // 将TempBuff追加到strBuff结尾
    }
    strncpy(OutputStr, strBuff, BuffLen * 2); // 将strBuff复制到OutputStr
    return BuffLen * 2;
}

uint8_t mcu_wait_get_userinfo(int timeout)
{
    uint16_t cnt = 0;
    uint8_t result = 0;
    while (cnt <= (timeout / 200))
    {
        WDOG_FEED;
        Radio.IrqProcess();
        LED_ON;
        HAL_Delay(100);
        LED_OFF;
        HAL_Delay(100);
        cnt++;
        if (SX1262_State == RX_Done)
        {
            if (RF_RX_Buffer[0] == 0xbb)
            {
                char get_sn[10] = {0};
                ArrayToStr(&RF_RX_Buffer[1], 5, get_sn);
                if (!strcmp(get_sn, SN))
                {
                    result = 1;
                    break;
                }
            }
        }
    }
    HAL_Delay(100); // 此延时绝不可删去,否则解析可能出错
    if (result)
    {
        /*TODO:处理数据*/
        info.ampThreshold = RF_RX_Buffer[6] << 8 | RF_RX_Buffer[7];
        info.freqThreshold = RF_RX_Buffer[8] << 8 | RF_RX_Buffer[9];
        info.sampleTime = RF_RX_Buffer[10] << 24 | RF_RX_Buffer[11] << 16 | RF_RX_Buffer[12] << 8 | RF_RX_Buffer[13];
        info.lowSendTime = RF_RX_Buffer[14] << 24 | RF_RX_Buffer[15] << 16 | RF_RX_Buffer[16] << 8 | RF_RX_Buffer[17];
        info.highSendTime = RF_RX_Buffer[18] << 24 | RF_RX_Buffer[19] << 16 | RF_RX_Buffer[20] << 8 | RF_RX_Buffer[21];
        FlashWrite(FLASH_SAVE_ADDR, (uint16_t *)&info, sizeof(info)); // 读取flash中的配置信息
        return 1;
    }
    else
    {
        return 0;
    }
}

/*************************************************
Function: 		hex2string
Description: 	hex字符数组转换成string字符串，用于printf显示等
Input: 			hex:原字符数组
                len:hex数组的长度
Output: 		ascII:输出的字符串
                newlen:输出的字符串长度
Return:
*************************************************/
static void hex2string(char *hex, char *ascII, int len, int *newlen)
{
    int i = 0;
    char newchar[100] = {0};
    *newlen = len * 3;
    for (i = 0; i < len; i++)
    {
        sprintf(newchar, "%02X ", hex[i]);
        strcat(ascII, newchar);
    }
}
/*************************************************
Function: 		string2hex
Description: 	字符串转换成hex,要求str只能是大写字母ABCDEF和数字
Input: 			str:要转换的字符串
Output: 		hex:转换后的hex字符数组
Return: 		0 成功
                1 不符合规则，失败
*************************************************/
static int string2hex(char *str, char *hex)
{
    int i = 0;
    int j = 0;
    unsigned char temp = 0;
    int str_len = 0;
    char str_cpy[100] = {'0'};
    strcpy(str_cpy, str);
    str_len = strlen(str_cpy);
    if (str_len == 0)
    {
        return 1;
    }
    while (i < str_len)
    {
        if (str_cpy[i] >= '0' && str_cpy[i] <= 'F')
        {
            if ((str_cpy[i] >= '0' && str_cpy[i] <= '9'))
            {
                temp = (str_cpy[i] & 0x0f) << 4;
            }
            else if (str_cpy[i] >= 'A' && str_cpy[i] <= 'F')
            {
                temp = ((str_cpy[i] + 0x09) & 0x0f) << 4;
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
        i++;
        if (str_cpy[i] >= '0' && str_cpy[i] <= 'F')
        {
            if (str_cpy[i] >= '0' && str_cpy[i] <= '9')
            {
                temp |= (str_cpy[i] & 0x0f);
            }
            else if (str_cpy[i] >= 'A' && str_cpy[i] <= 'F')
            {
                temp |= ((str_cpy[i] + 0x09) & 0x0f);
            }
            else
            {
                return 1;
            }
        }
        else
        {
            return 1;
        }
        i++;
        hex[j] = temp;
        j++;
    }
    return 0;
}

extern uint8_t bat;                                    // 电池电量
extern float freq;                                     // 频率
extern float ampl;                                     // 振幅
#define ntohs(x) (0xff00 & x << 8) | (0x00ff & x >> 8) // 大小端转换
void generateFrame(void)
{
    if (!string2hex(SN, (char *)&loraObject.txFrame.sn[1]))
    {
        loraObject.txFrame.sn[0] = 0xaa;
        loraObject.txFrame.len = sizeof(_Frame_Tx);
        loraObject.txFrame.battery = bat;
        loraObject.txFrame.frequency = (uint16_t)stackValue.freq;
        loraObject.txFrame.frequency = ntohs(loraObject.txFrame.frequency);
        loraObject.txFrame.amplitude = (uint16_t)stackValue.ampl;
        loraObject.txFrame.amplitude = ntohs(loraObject.txFrame.amplitude);
        loraObject.txFrame.crc16 = Protocol_Crc16((uint8_t *)&loraObject.txFrame, sizeof(_Frame_Tx) - 2);
        loraObject.txFrame.crc16 = ntohs(loraObject.txFrame.crc16);
    }
}

uint16_t Protocol_Crc16(uint8_t *dat, uint8_t lenth)
{
    unsigned char i, _Flag, _LSB = 0;
    unsigned int CRC_Value = 0xFFFF;
    for (i = 0; i < lenth; i++) //=-1
    {
        CRC_Value ^= dat[i];
        for (_Flag = 0; _Flag <= 7; _Flag++)
        {
            _LSB = (unsigned char)(CRC_Value & 0x0001);
            CRC_Value = CRC_Value >> 1;
            if (_LSB == 1)
                CRC_Value ^= 0xA001;
        }
    }
    return CRC_Value;
}

void writeRtcBKP(void)
{
    stackValue.sampleSec += 10;
    stackValue.sendSec += 10;
    stackValue.firstInitiate = 1; // 表示下一次则不是第一次上电
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, stackValue.freq);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, stackValue.ampl);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, stackValue.sampleSec);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, stackValue.sendSec);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, stackValue.firstInitiate);
    HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR6, stackValue.bat);
}

void readRtcBKP(void)
{
    stackValue.freq = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1);
    stackValue.ampl = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
    stackValue.sampleSec = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
    stackValue.sendSec = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
    stackValue.firstInitiate = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
    stackValue.bat = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
}

void mcu_rtc_alarmSet(uint32_t Seconds)
{
    // 说明： Seconds 不能超过 24 * 60 * 60   不能超过一天
    RTC_AlarmTypeDef RTC_AlarmStructure;
    /* RTC 闹钟中断配置 */
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 1, 0);
    /* 使能RTC闹钟中断 */
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
    /* 设置闹钟时间 */
    RTC_DateTypeDef sdatestructureget;
    RTC_TimeTypeDef stimestructureget;
    /* Get the RTC current Time */
    HAL_RTC_GetTime(&hrtc, &stimestructureget, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sdatestructureget, RTC_FORMAT_BIN);
    RTC_AlarmStructure.Alarm = RTC_ALARM_A;
    RTC_AlarmStructure.AlarmTime.Hours = (Seconds / 3600 + stimestructureget.Hours) / 10 * 16 + (Seconds / 3600 + stimestructureget.Hours) % 10;
    RTC_AlarmStructure.AlarmTime.Minutes = ((Seconds % 3600) / 60 + stimestructureget.Minutes) / 10 * 16 + ((Seconds % 3600) / 60 + stimestructureget.Minutes) % 10;
    RTC_AlarmStructure.AlarmTime.Seconds = (Seconds % 60 + stimestructureget.Seconds) / 10 * 16 + (Seconds % 60 + stimestructureget.Seconds) % 10;
    HAL_RTC_SetAlarm_IT(&hrtc, &RTC_AlarmStructure, RTC_FORMAT_BCD);
}