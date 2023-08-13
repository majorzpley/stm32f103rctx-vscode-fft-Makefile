#include "sx126x_Init.h"
#include "gpio.h"
#include "radio.h"
#include "stdio.h"
#include "string.h"
#include "sx126x.h"

/*!
 * Radio events function pointer
 * 这个是传参进入其他函数中了，所以用全局变量(局部变量使用完了内存释放可能导致异常)
 */

static void delay1ms(uint32_t time)
{
    HAL_Delay(time);
}
static RadioEvents_t SX126xRadioEvents;

static void SX126xOnTxDone(void);
static void SX126xOnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr);
static void SX126xOnTxTimeout(void);
static void SX126xOnRxTimeout(void);
static void SX126xOnRxError(void);
static void OnCadDone(bool channelActivityDetected);

States_t SX1262_State;
uint8_t OCP_Value = 0;

// 开启一个定时发送任务，每隔1S发送一条数据

void Radio_Init_RXD(void)
{
    SX126xRadioEvents.TxDone = SX126xOnTxDone;
    SX126xRadioEvents.RxDone = SX126xOnRxDone;
    SX126xRadioEvents.TxTimeout = SX126xOnTxTimeout;
    SX126xRadioEvents.RxTimeout = SX126xOnRxTimeout;
    SX126xRadioEvents.RxError = SX126xOnRxError;

    Radio.Init(&SX126xRadioEvents);
    Radio.SetChannel(520000000);
#if defined(USE_MODEM_LORA)
    // 参数：lora模式,发射功率,fsk用的lora设置为0就可以，带宽，纠错编码率，前导码长度，固定长度数据包(一般是不固定的所以选false)，crc校验，0表示关闭跳频，跳频之间的符号数(关闭跳频这个参数没有意义)，这个应该是表示是否要翻转中断电平的，超时时间

    Radio.SetTxConfig(MODEM_LORA, 20, 0, 0, 9, LORA_CODINGRATE, LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON, true,
                      0, 0, LORA_IQ_INVERSION_ON, 3000);

    Radio.SetRxConfig(MODEM_LORA, 0, 9, LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH, LORA_SX126X_SYMBOL_TIMEOUT,
                      LORA_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

#elif defined(USE_MODEM_FSK)
    Radio.SetTxConfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0, FSK_DATARATE, 0, FSK_PREAMBLE_LENGTH,
                      FSK_FIX_LENGTH_PAYLOAD_ON, true, 0, 0, 0, 3000);

    Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE, 0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH, 0,
                      FSK_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0, false, true);

#else
#error "Please define a frequency band in the compiler options."
#endif
    OCP_Value = Radio.Read(REG_OCP);
    Radio.Standby();
    Radio.Rx(0);
}

void Radio_Init(void)
{
    SX126xRadioEvents.TxDone = SX126xOnTxDone;
    SX126xRadioEvents.RxDone = SX126xOnRxDone;
    SX126xRadioEvents.TxTimeout = SX126xOnTxTimeout;
    SX126xRadioEvents.RxTimeout = SX126xOnRxTimeout;
    SX126xRadioEvents.RxError = SX126xOnRxError;

    Radio.Init(&SX126xRadioEvents);
    Radio.SetChannel(LORA_TX_FRE);
#if defined(USE_MODEM_LORA)
    // 参数：lora模式,发射功率,fsk用的lora设置为0就可以，带宽，纠错编码率，前导码长度，固定长度数据包(一般是不固定的所以选false)，crc校验，0表示关闭跳频，跳频之间的符号数(关闭跳频这个参数没有意义)，这个应该是表示是否要翻转中断电平的，超时时间

    Radio.SetTxConfig(MODEM_LORA, LORA_TX_OUTPUT_POWER, 0, LORA_BANDWIDTH, LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                      LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON, true, 0, 0, LORA_IQ_INVERSION_ON, 3000);

    Radio.SetRxConfig(MODEM_LORA, 0, LORA_SPREADING_FACTOR, LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                      LORA_SX126X_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0, LORA_IQ_INVERSION_ON,
                      true);

#elif defined(USE_MODEM_FSK)
    Radio.SetTxConfig(MODEM_FSK, TX_OUTPUT_POWER, FSK_FDEV, 0, FSK_DATARATE, 0, FSK_PREAMBLE_LENGTH,
                      FSK_FIX_LENGTH_PAYLOAD_ON, true, 0, 0, 0, 3000);

    Radio.SetRxConfig(MODEM_FSK, FSK_BANDWIDTH, FSK_DATARATE, 0, FSK_AFC_BANDWIDTH, FSK_PREAMBLE_LENGTH, 0,
                      FSK_FIX_LENGTH_PAYLOAD_ON, 0, true, 0, 0, false, true);

#else
#error "Please define a frequency band in the compiler options."
#endif
    OCP_Value = Radio.Read(REG_OCP);
    Radio.Standby();
    Radio.Rx(0);
}
uint8_t RF_TX_Done = 0;
static void SX126xOnTxDone(void)
{
    //	Radio.Standby();
    //	Radio.Rx( LORA_RX_TIMEOUT_VALUE );
    Radio.SetChannel(LORA_RX_FRE); // 接收频率
    Radio.Standby();
    Radio.Rx(LORA_RX_TIMEOUT_VALUE);
    SX1262_State = TX_Done;
}

int8_t RssiValue = 0;
int8_t SnrValue = 0;
uint16_t RF_RX_Count = 256;
unsigned char RF_RX_Buffer[256];
uint8_t RF_RX_Done = 0;
bool Cad_Done = false;

static void SX126xOnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr)
{
    Radio.Standby();

    RF_RX_Count = size;
    memcpy(RF_RX_Buffer, payload, RF_RX_Count);
    RssiValue = rssi;
    SnrValue = snr;

    Radio.Rx(LORA_RX_TIMEOUT_VALUE);
    SX1262_State = RX_Done;
}

static void SX126xOnTxTimeout(void)
{
    Radio.Standby();
    Radio.Rx(LORA_RX_TIMEOUT_VALUE);

    SX1262_State = TX_TIMEOUT;
}

static void SX126xOnRxTimeout(void)
{
    Radio.Standby();

    Radio.Rx(LORA_RX_TIMEOUT_VALUE);
    SX1262_State = RX_TIMEOUT;
}

static void SX126xOnRxError(void)
{
    Radio.Standby();

    Radio.Rx(LORA_RX_TIMEOUT_VALUE);
    SX1262_State = RX_ERROR;
}

static void OnCadDone(bool channelActivityDetected)
{
    Cad_Done = channelActivityDetected;
}

void SX126xDelayMs(void)
{
    delay1ms(1);
}

// 射频发送函数//
void Radio_Send(uint8_t *pData, unsigned char len, uint16_t time_out)
{
    Radio.Send(pData, len);
    SX1262_State = Null;
    while ((SX1262_State != TX_Done) && (time_out > 1))
    {
        delay1ms(1);
        time_out--;
        if (IS_RF_NIRQ == 1)
        {
            Radio_IrqProcess();
        }
    }
}

// 射频接收函数//
void Radio_Recev(uint16_t time_out)
{
    Radio.Standby();
    Radio.Rx(0);
    SX1262_State = Null;
    while ((SX1262_State != RX_Done) && (time_out > 1))
    {
        delay1ms(1);
        time_out--;
        Radio_IrqProcess();
    }
}

void Radio_IrqProcess(void)
{
    Radio.IrqProcess();
}

void Radio_startCad(uint16_t count)
{
    while (count > 1)
    {
        Radio.StartCad(); // 开启CAD模式
        delay1ms(50);

        Radio_IrqProcess(); // 检测CAD

        if (Cad_Done == false) // 检测完成,信道空闲
        {
            count = 0;
        }
        else // 信道忙
        {
            delay1ms(10);
            count--;
        }
    }
}

uint8_t RF_NIRQ = 0;
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
//{
//	if(GPIO_PIN == GPIO_PIN_0)
//	{
//		//__HAL_TIM_CLEAR_FLAG(&htim2,TIM_FLAG_TRIGGER);
//		__HAL_GPIO_EXTI_CLEAR_IT(EXTI_LINE_0);
//		//HAL_TIM_Base_Stop_IT(&htim2);
//		RF_NIRQ = 1;
//		Radio_IrqProcess();
//
//	}

//}
