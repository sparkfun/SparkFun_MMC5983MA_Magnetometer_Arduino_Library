/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file implements all functions used in the MMC5983MA High Performance Magnetometer Arduino Library IO layer.

  SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
  See LICENSE.md for more information.
*/

#include "SparkFun_MMC5983MA_IO.h"
#include "SparkFun_MMC5983MA_Arduino_Library_Constants.h"

// Read operations must have the most significant bit set
#define READ_REG(x) (0x80 | x)

bool SFE_MMC5983MA_IO::begin(TwoWire &i2cPort)
{
    useSPI = false;
    _i2cPort = &i2cPort;
    return isConnected();
}

void SFE_MMC5983MA_IO::initSPISettings()
{
	//CPOL = 1, CPHA = 1 : SPI Mode 3 according to datasheet
	// In practice SPI_MODE0 is what worked.
	_mmcSpiSettings = SPISettings(2000000, MSBFIRST, SPI_MODE0); 
}


bool SFE_MMC5983MA_IO::begin(const uint8_t csPin, SPIClass &spiPort)
{
    useSPI = true;
    _csPin = csPin;
    digitalWrite(_csPin, HIGH);
    pinMode(_csPin, OUTPUT);
    _spiPort = &spiPort;

		initSPISettings();

    return isConnected();
}

bool SFE_MMC5983MA_IO::begin(const uint8_t csPin, SPISettings userSettings, SPIClass &spiPort)
{
    useSPI = true;
    _csPin = csPin;
    digitalWrite(_csPin, HIGH);
    pinMode(_csPin, OUTPUT);
    _spiPort = &spiPort;

		_mmcSpiSettings = userSettings;

    return isConnected();
}

bool SFE_MMC5983MA_IO::isConnected()
{
    bool result = false;
    if (useSPI)
    {
				_spiPort->beginTransaction(_mmcSpiSettings);
        digitalWrite(_csPin, LOW);
        _spiPort->transfer(READ_REG(PROD_ID_REG));
        uint8_t readback = _spiPort->transfer(DUMMY);
        digitalWrite(_csPin, HIGH);
        result = (readback == PROD_ID);
    }
    else
    {
        _i2cPort->beginTransmission(I2C_ADDR);
        if (_i2cPort->endTransmission() == 0)
            result = readSingleByte(PROD_ID_REG) == PROD_ID;
    }
    return result;
}

void SFE_MMC5983MA_IO::writeMultipleBytes(const uint8_t registerAddress, uint8_t *const buffer, uint8_t const packetLength)
{
    if (useSPI)
    {
				_spiPort->beginTransaction(_mmcSpiSettings);
        digitalWrite(_csPin, LOW);
        _spiPort->transfer(registerAddress);
        _spiPort->transfer(buffer, packetLength);
        digitalWrite(_csPin, HIGH);
    }
    else
    {
        _i2cPort->beginTransmission(I2C_ADDR);
        _i2cPort->write(registerAddress);
        for (uint8_t i = 0; i < packetLength; i++)
            _i2cPort->write(buffer[i]);

        _i2cPort->endTransmission();
    }
}

void SFE_MMC5983MA_IO::readMultipleBytes(const uint8_t registerAddress, uint8_t *const buffer, const uint8_t packetLength)
{
    if (useSPI)
    {
				_spiPort->beginTransaction(_mmcSpiSettings);
        digitalWrite(_csPin, LOW);
        _spiPort->transfer(READ_REG(registerAddress));
        _spiPort->transfer(buffer, packetLength);
        digitalWrite(_csPin, HIGH);
    }
    else
    {
        _i2cPort->beginTransmission(I2C_ADDR);
        _i2cPort->write(registerAddress);
        _i2cPort->endTransmission();

        _i2cPort->requestFrom(I2C_ADDR, packetLength);
        for (uint8_t i = 0; (i < packetLength); i++)
            buffer[i] = _i2cPort->read();
    }
}

uint8_t SFE_MMC5983MA_IO::readSingleByte(const uint8_t registerAddress)
{
    uint8_t result = 0;
    if (useSPI)
    {
				_spiPort->beginTransaction(_mmcSpiSettings);
        digitalWrite(_csPin, LOW);
        _spiPort->transfer(READ_REG(registerAddress));
        result = _spiPort->transfer(DUMMY);
        digitalWrite(_csPin, HIGH);
    }
    else
    {
        _i2cPort->beginTransmission(I2C_ADDR);
        _i2cPort->write(registerAddress);
        _i2cPort->endTransmission();
        _i2cPort->requestFrom(I2C_ADDR, 1U);
        result = _i2cPort->read();
    }
    return result;
}

void SFE_MMC5983MA_IO::writeSingleByte(const uint8_t registerAddress, const uint8_t value)
{
    if (useSPI)
    {
				_spiPort->beginTransaction(_mmcSpiSettings);
        digitalWrite(_csPin, LOW);
        _spiPort->transfer(registerAddress);
        _spiPort->transfer(value);
        digitalWrite(_csPin, HIGH);
    }
    else
    {
        _i2cPort->beginTransmission(I2C_ADDR);
        _i2cPort->write(registerAddress);
        _i2cPort->write(value);
        _i2cPort->endTransmission();
    }
}

void SFE_MMC5983MA_IO::setRegisterBit(const uint8_t registerAddress, const uint8_t bitMask)
{
    uint8_t value = readSingleByte(registerAddress);
    value |= bitMask;
    writeSingleByte(registerAddress, value);
}

void SFE_MMC5983MA_IO::clearRegisterBit(const uint8_t registerAddress, const uint8_t bitMask)
{
    uint8_t value = readSingleByte(registerAddress);
    value &= ~bitMask;
    writeSingleByte(registerAddress, value);
}

bool SFE_MMC5983MA_IO::isBitSet(const uint8_t registerAddress, const uint8_t bitMask)
{
    uint8_t value = readSingleByte(registerAddress);
    return (value & bitMask);
}

bool SFE_MMC5983MA_IO::spiInUse()
{
    return useSPI;
}
