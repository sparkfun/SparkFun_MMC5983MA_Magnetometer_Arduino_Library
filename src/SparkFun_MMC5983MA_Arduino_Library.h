/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file declares all functions used in the MMC5983MA High Performance Magnetometer Arduino Library.

  SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
  See LICENSE.md for more information.
*/

#ifndef _SPARKFUN_MMC5983MA_
#define _SPARKFUN_MMC5983MA_

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include "SparkFun_MMC5983MA_IO.h"
#include "SparkFun_MMC5983MA_Arduino_Library_Constants.h"

class SFE_MMC5983MA
{
private:
  // I2C communication object instance.
  SFE_MMC5983MA_IO mmc_io;
  // Error callback function pointer.
  // Function must accept a SF_MMC5983MA_ERROR as errorCode.
  void (*errorCallback)(SF_MMC5983MA_ERROR errorCode) = nullptr;

  // Since some registers are write-only in MMC5983MA all operations
  // are done in shadow memory locations. Default reset values are
  // set to the shadow memory locations upon initialization and after
  // any bit set in the shadow location the register is atomically written.
  struct MemoryShadow
  {
    uint8_t internalControl0 = 0x0;
    uint8_t internalControl1 = 0x0;
    uint8_t internalControl2 = 0x0;
    uint8_t internalControl3 = 0x0;
  } memoryShadow;

  // Sets register bit(s) on memory shadows and then registers (if doWrite is true)
  bool setShadowBit(uint8_t registerAddress, const uint8_t bitMask, bool doWrite = true);

  // Clears register bit(s) on memory shadows and then registers (if doWrite is true)
  bool clearShadowBit(uint8_t registerAddress, const uint8_t bitMask, bool doWrite = true);

  // Checks if a specific bit is set on a register memory shadow
  bool isShadowBitSet(uint8_t registerAddress, const uint8_t bitMask);

  // Return a timeout for getMeasurement based on BW1/0
  uint16_t getTimeout();

public:
  // Default constructor.
  SFE_MMC5983MA() = default;

  // Default destructor.
  ~SFE_MMC5983MA() = default;

  // Sets the error callback function.
  void setErrorCallback(void (*errorCallback)(SF_MMC5983MA_ERROR errorCode));

  // Convert errorCode to text
  const char *errorCodeString(SF_MMC5983MA_ERROR errorCode);
  
  // Initializes MMC5983MA using I2C
  bool begin(TwoWire &wirePort = Wire);

  // Initializes MMC5983MA using SPI
  bool begin(uint8_t csPin, SPIClass& spiPort = SPI);
  bool begin(uint8_t csPin, SPISettings userSettings, SPIClass& spiPort = SPI);

  // Polls if MMC5983MA is connected and if chip ID matches MMC5983MA chip id.
  bool isConnected();

  // Returns die temperature. Range is -75C to 125C.
  int getTemperature();

  // Soft resets the device.
  bool softReset();

  // Enables interrupt generation after measurement is completed.
  // Must be re-enabled after each measurement.
  bool enableInterrupt();

  // Disables interrupt generation.
  bool disableInterrupt();

  // Checks if interrupt generation is enabled.
  bool isInterruptEnabled();

  // Enables 3 wire SPI interface
  bool enable3WireSPI();

  // Disables SPI interface
  bool disable3WireSPI();

  // Checks if SPI is enabled
  bool is3WireSPIEnabled();

  // Performs SET operation
  bool performSetOperation();

  // Performs RESET operation
  bool performResetOperation();

  // Enables automatic SET/RESET
  bool enableAutomaticSetReset();

  // Disables automatic SET/RESET
  bool disableAutomaticSetReset();

  // Checks if automatic SET/RESET is enabled
  bool isAutomaticSetResetEnabled();

  // Enables X channel output
  bool enableXChannel();

  // Disables X channel output
  bool disableXChannel();

  // Checks if X channel output is enabled
  // Note: this returns true when the X channel is inhibited.
  // Strictly, it should be called isXChannelInhibited.
  bool isXChannelEnabled();

  // Enables Y and Z channels outputs
  bool enableYZChannels();

  // Disables Y and Z channels outputs
  bool disableYZChannels();

  // Checks if YZ channels outputs are enabled
  // Note: this returns true when the Y and Z channels are inhibited.
  // Strictly, it should be called areYZChannelsInhibited.
  bool areYZChannelsEnabled();

  // Sets decimation filter bandwidth. Allowed values are 800, 400, 200 or 100. Defaults to 100 on invalid values.
  bool setFilterBandwidth(uint16_t bandwidth);

  // Gets current decimation filter bandwidth. Values are in Hz.
  uint16_t getFilterBandwidth(); // Typo fix - #17
  uint16_t getFilterBandwith() { return getFilterBandwidth(); } // Retained for backward compatibility

  // Enables continuous mode. Continuous mode frequency must be greater than 0.
  bool enableContinuousMode();

  // Disables continuous mode.
  bool disableContinuousMode();

  // Checks if continuous mode is enabled.
  bool isContinuousModeEnabled();

  // Sets continuous mode frequency. Allowed values are 1000, 200, 100, 50, 20, 10, 1 and 0 (off). Defaults to 0 (off).
  bool setContinuousModeFrequency(uint16_t frequency);

  // Gets continuous mode frequency.
  uint16_t getContinuousModeFrequency();

  // Enables periodic set
  bool enablePeriodicSet();

  // Disables periodic set
  bool disablePeriodicSet();

  // Checks if periodic set is enabled
  bool isPeriodicSetEnabled();  

  // Sets how often the chip will perform an automatic set operation. Allowed values are 1, 25, 75, 100, 250, 500, 1000, 2000. Defaults to 1.
  bool setPeriodicSetSamples(uint16_t numberOfSamples);

  // Gets how many times the chip is performing an automatic set operation.
  uint16_t getPeriodicSetSamples();

  // Apply extra current from positive side to negative side of the coil. This feature can be used to check whether the sensor has been saturated.
  bool applyExtraCurrentPosToNeg();

  // Remove extra current from positive side to negative side of the coil.
  bool removeExtraCurrentPosToNeg();

  // Checks if extra current is applied from positive to negative side of coil.
  bool isExtraCurrentAppliedPosToNeg();

  // Apply extra current from negative side to positive side of the coil. This feature can be used to check whether the sensor has been saturated.
  bool applyExtracurrentNegToPos();

  // Remove extra current from negative side to positive side of the coil.
  bool removeExtracurrentNegToPos();

  // Checks if extra current is applied from negative to positive side of coil.
  bool isExtraCurrentAppliedNegToPos();

  // Get X axis measurement
  uint32_t getMeasurementX();

  // Get Y axis measurement
  uint32_t getMeasurementY();

  // Get Z axis measurement
  uint32_t getMeasurementZ();

  // Get X, Y and Z field strengths in a single measurement
  bool getMeasurementXYZ(uint32_t *x, uint32_t *y, uint32_t *z);

  // Read and return the X, Y and Z field strengths
  bool readFieldsXYZ(uint32_t *x, uint32_t *y, uint32_t *z);

  // Clear the Meas_T_Done and/or Meas_M_Done interrupts
  // By default, clear both
  bool clearMeasDoneInterrupt(uint8_t measMask = MEAS_T_DONE | MEAS_M_DONE);
};

#endif