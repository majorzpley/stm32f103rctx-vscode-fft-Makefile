#include "fft_handle.h"
#include "main.h"
#include "double_integral.h"
uint16_t ADC1_buff[Samp1_Point] = {0};
uint16_t ADC2_buff[Samp2_Point] = {0};

void Delay(unsigned int n)
{
    unsigned int i;
    for (i = 0; i < n; i++)
        ;
    return;
}
// 模拟生成读取时钟信号
static void SampleClk(void)
{
    SAMPLE_SCK_0;
    Delay(DELAYMS);
    SAMPLE_SCK_1;
    Delay(DELAYMS);
    SAMPLE_SCK_0;
    Delay(DELAYMS);
    SAMPLE_SCK_1;
    Delay(DELAYMS);
    SAMPLE_SCK_0;
    Delay(DELAYMS);
    SAMPLE_SCK_1;
    Delay(DELAYMS);
    SAMPLE_SCK_0;
    Delay(DELAYMS);
    SAMPLE_SCK_1;
    Delay(DELAYMS);
    SAMPLE_SCK_0;
    Delay(DELAYMS);
    SAMPLE_SCK_1;
    Delay(DELAYMS);
    SAMPLE_SCK_0;
    Delay(DELAYMS);
    SAMPLE_SCK_1;
    Delay(DELAYMS);
}

void ADRead(uint16_t *adc1_data, uint16_t *adc2_data)
{
    // EN_5V_ENABLE;
    unsigned int i;
    unsigned int tempbit = 0;
    unsigned int data1 = 0, data2 = 0;
    SAMPLE_CS_O;
    SampleClk();
    SAMPLE_SCK_0;
    for (i = 0; i < 16; i++)
    {
        Delay(DELAYMS);
        SAMPLE_SCK_1;
        if (SAMPLE_DI_1)
        {
            tempbit = 1;
        }
        else
        {
            tempbit = 0;
        }
        data1 = (data1 << 1) | tempbit;

        if (SAMPLE_DI_2)
        {
            tempbit = 1;
        }
        else
        {
            tempbit = 0;
        }
        data2 = (data2 << 1) | tempbit;
        SAMPLE_SCK_0;
    }
    SAMPLE_CS_1;
    if (adc1_data)
        *adc1_data = data1;
    if (adc2_data)
        *adc2_data = data2;
    // EN_5V_DISABLE;
}

float adc_temp[1024] = {0};
// BUG:运用fft计算出频率和幅度
void fftGetFreqAmplitude(uint16_t *input, uint16_t samplePoints, float *freq, float *amplitude)
{
    for (size_t i = 0; i < samplePoints; i++)
    {
        // 计算出每个点的电压值±2.5V
        adc_temp[i] = (float)(input[i] / 65535.0 * 5.0 - 2.497);
    }
    // 采样点数：2048，积分次数：1
    integ_init(samplePoints, 1);
    frq_domain_integral(adc_temp, NULL, amplitude, freq);
}
