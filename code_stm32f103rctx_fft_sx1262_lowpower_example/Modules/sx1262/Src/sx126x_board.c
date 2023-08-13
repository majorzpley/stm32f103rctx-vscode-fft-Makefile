/*
  ______                              _
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
    (C)2013 Semtech

Description: SX126x driver specific target board functions implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/
#include "gpio.h"
#include "spi.h"
#include "radio.h"
#include "sx126x.h"
#include "sx126x-board.h"
// #include "App_gpio.h"
void HAL_Delay_nMS(uint16_t time)
{
    // delay1ms(time);
    HAL_Delay(time);
}

void SX126xReset(void)
{
    HAL_Delay_nMS(10);
    // Gpio_ClrIO(RF_RST_PORT, RF_RST_PIN);
    RF_RST_L;
    HAL_Delay_nMS(20);
    //   Gpio_SetIO(RF_RST_PORT, RF_RST_PIN);
    RF_RST_H;
    HAL_Delay_nMS(10);
}

/**
 * 设置SPI的片选脚的电平(低电平使能，高电平取消)
 * lev:true的时候高电平,false的时候低电平
 */
void Sx1276SetNSS(bool lev)
{
    if (lev)
    {
        // Spi_SetCS(M0P_SPI0, TRUE);
        RF_CS_H;
    }
    else
    {
        // Spi_SetCS(M0P_SPI0, FALSE);
        RF_CS_L;
    }
}

/**
 * SPI读写函数,data是发送的数据，返回值是接收到的数据
 */
uint8_t Sx1276SpiInOut(uint16_t data)
{
    return SpiInOut(data);
}

void SX126xWaitOnBusy(void)
{
    // whileV (Gpio_GetInputIO(RF_BUSY_PORT, RF_BUSY_PIN) == 1)
    while (IS_RF_BUSY == 1)
        ;
}

void SX126xWakeup(void)
{

    Sx1276SetNSS(0);
    Sx1276SpiInOut(RADIO_GET_STATUS);
    Sx1276SpiInOut(0);

    Sx1276SetNSS(1);

    // Wait for chip to be ready.
    SX126xWaitOnBusy();
}

void SX126xWriteCommand(RadioCommands_t command, uint8_t *buffer, uint16_t size)
{

    SX126xCheckDeviceReady();

    Sx1276SetNSS(0);

    Sx1276SpiInOut((uint8_t)command);

    for (uint16_t i = 0; i < size; i++)
    {
        Sx1276SpiInOut(buffer[i]);
    }

    Sx1276SetNSS(1);

    if (command != RADIO_SET_SLEEP)
    {
        SX126xWaitOnBusy();
    }
}

void SX126xReadCommand(RadioCommands_t command, uint8_t *buffer, uint16_t size)
{
    SX126xCheckDeviceReady();

    Sx1276SetNSS(0);

    Sx1276SpiInOut((uint8_t)command);
    Sx1276SpiInOut(0x00);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = Sx1276SpiInOut(0);
    }

    Sx1276SetNSS(1);

    SX126xWaitOnBusy();
}

void SX126xWriteRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    SX126xCheckDeviceReady();

    Sx1276SetNSS(0);

    Sx1276SpiInOut(RADIO_WRITE_REGISTER);
    Sx1276SpiInOut((address & 0xFF00) >> 8);
    Sx1276SpiInOut(address & 0x00FF);

    for (uint16_t i = 0; i < size; i++)
    {
        Sx1276SpiInOut(buffer[i]);
    }

    Sx1276SetNSS(1);

    SX126xWaitOnBusy();
}

void SX126xWriteRegister(uint16_t address, uint8_t value)
{
    SX126xWriteRegisters(address, &value, 1);
}

void SX126xReadRegisters(uint16_t address, uint8_t *buffer, uint16_t size)
{
    SX126xCheckDeviceReady();

    Sx1276SetNSS(0);

    Sx1276SpiInOut(RADIO_READ_REGISTER);
    Sx1276SpiInOut((address & 0xFF00) >> 8);
    Sx1276SpiInOut(address & 0x00FF);
    Sx1276SpiInOut(0);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = Sx1276SpiInOut(0);
    }

    Sx1276SetNSS(1);

    SX126xWaitOnBusy();
}

uint8_t SX126xReadRegister(uint16_t address)
{
    uint8_t data;
    SX126xReadRegisters(address, &data, 1);
    return data;
}

void SX126xWriteBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    SX126xCheckDeviceReady();

    Sx1276SetNSS(0);

    Sx1276SpiInOut(RADIO_WRITE_BUFFER);
    Sx1276SpiInOut(offset);
    for (uint16_t i = 0; i < size; i++)
    {
        Sx1276SpiInOut(buffer[i]);
    }

    Sx1276SetNSS(1);

    SX126xWaitOnBusy();
}

void SX126xReadBuffer(uint8_t offset, uint8_t *buffer, uint8_t size)
{
    SX126xCheckDeviceReady();

    Sx1276SetNSS(0);

    Sx1276SpiInOut(RADIO_READ_BUFFER);
    Sx1276SpiInOut(offset);
    Sx1276SpiInOut(0);
    for (uint16_t i = 0; i < size; i++)
    {
        buffer[i] = Sx1276SpiInOut(0);
    }

    Sx1276SetNSS(1);

    SX126xWaitOnBusy();
}

void SX126xSetRfTxPower(int8_t power)
{
    SX126xSetTxParams(power, RADIO_RAMP_40_US);
}

uint8_t SX126xGetPaSelect(uint32_t channel)
{
    //    if( GpioRead( &DeviceSel ) == 1 )
    //    {
    //        return SX1261;
    //    }
    //    else
    //    {
    //        return SX1262;
    //    }

    return SX1262;
}

void SX126xAntSwOn(void)
{
    // GpioInit( &AntPow, ANT_SWITCH_POWER, PIN_OUTPUT, PIN_PUSH_PULL, PIN_PULL_UP, 1 );
}

void SX126xAntSwOff(void)
{
    // GpioInit( &AntPow, ANT_SWITCH_POWER, PIN_ANALOGIC, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
}

bool SX126xCheckRfFrequency(uint32_t frequency)
{
    // Implement check. Currently all frequencies are supported
    return true;
}
