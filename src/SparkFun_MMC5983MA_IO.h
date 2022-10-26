/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file declares all functions used in the MMC5983MA High Performance Magnetometer Arduino Library I2C/SPI IO layer.

  SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
  See LICENSE.md for more information.
*/

#ifndef _SPARKFUN_MMC5983MA_IO_
#define _SPARKFUN_MMC5983MA_IO_

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class SFE_MMC5983MA_IO
{
private:
  SPIClass *_spiPort = nullptr;
  uint8_t _csPin = 0;
  SPISettings _mmcSpiSettings;

  TwoWire *_i2cPort = nullptr;
  uint8_t _address = 0;
  bool useSPI = false;

public:
  // Default empty constructor.
  SFE_MMC5983MA_IO() = default;

  // Default empty destructor
  ~SFE_MMC5983MA_IO() = default;

  // Builds default SPI settings if none are provided.
  void initSPISettings();

  // Configures and starts the I2C I/O layer.
  bool begin(TwoWire &wirePort);

  // Configures and starts the SPI I/O layer.
  bool begin(const uint8_t csPin, SPIClass &spiPort = SPI);

  // Configures the SPI I/O layer with the given chip select and SPI settings provided by the user.
  bool begin(const uint8_t csPin, SPISettings userSettings, SPIClass &spiPort = SPI);

  // Returns true if we get the correct product ID from the device.
  bool isConnected();

  // Read a single uint8_t from a register.
  bool readSingleByte(const uint8_t registerAddress, uint8_t *buffer);

  // Writes a single uint8_t into a register.
  bool writeSingleByte(const uint8_t registerAddress, const uint8_t value);

  // Reads multiple bytes from a register into buffer uint8_t array.
  bool readMultipleBytes(const uint8_t registerAddress, uint8_t *const buffer, const uint8_t packetLength);

  // Writes multiple bytes to register from buffer uint8_t array.
  bool writeMultipleBytes(const uint8_t registerAddress, uint8_t *const buffer, const uint8_t packetLength);

  // Sets a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
  bool setRegisterBit(const uint8_t registerAddress, const uint8_t bitMask);

  // Clears a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
  bool clearRegisterBit(const uint8_t registerAddress, const uint8_t bitMask);

  // Returns true if a specific bit is set in a register. Bit position ranges from 0 (lsb) to 7 (msb).
  bool isBitSet(const uint8_t registerAddress, const uint8_t bitMask);

  // Returns true if the interface in use is SPI
  bool spiInUse();
};

#endif
