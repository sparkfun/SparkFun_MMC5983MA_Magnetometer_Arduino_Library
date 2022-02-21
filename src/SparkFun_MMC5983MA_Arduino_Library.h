/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file declares all functions used in the MMC5983MA High Performance Magnetometer Arduino Library.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
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

  // Sets register bit(s) on memory shadows and then registers
  void setShadowBit(uint8_t registerAddress, const uint8_t bitMask);

  // Clears register bit(s) on memory shadows and then registers
  void clearShadowBit(uint8_t registerAddress, const uint8_t bitMask);

  // Checks if a specific bit is set on a register memory shadow
  bool isShadowBitSet(uint8_t registerAddress, const uint8_t bitMask);

public:
  // Default constructor.
  SFE_MMC5983MA() = default;

  // Default destructor.
  ~SFE_MMC5983MA() = default;

  // Sets the error callback function.
  void setErrorCallback(void (*errorCallback)(SF_MMC5983MA_ERROR errorCode));

  // Initializes MMC5983MA using I2C
  bool begin(TwoWire &wirePort = Wire);

  // Initializes MMC5983MA using SPI
  bool begin(uint8_t csPin, SPIClass& spiPort = SPI);

  // Polls if MMC5983MA is connected and if chip ID matches MMC5983MA chip id.
  bool isConnected();

  // Returns die temperature. Range is -75C to 125C.
  int getTemperature();

  // Soft resets the device.
  void softReset();

  // Enables interrupt generation after measurement is completed.
  // Must be re-enabled after each measurement.
  void enableInterrupt();

  // Disables interrupt generation.
  void disableInterrupt();

  // Checks if interrupt generation is enabled.
  bool isInterruptEnabled();

  // Enables 3 wire SPI interface
  void enable3WireSPI();

  // Disables SPI interface
  void disable3WireSPI();

  // Checks if SPI is enabled
  bool is3WireSPIEnabled();

  // Performs SET operation
  void performSetOperation();

  // Performs RESET operation
  void performResetOperation();

  // Enables automatic SET/RESET
  void enableAutomaticSetReset();

  // Disables automatic SET/RESET
  void disableAutomaticSetReset();

  // Checks if automatic SET/RESET is enabled
  bool isAutomaticSetResetEnabled();

  // Enables X channel output
  void enableXChannel();

  // Disables X channel output
  void disableXChannel();

  // Checks if X channel output is enabled
  bool isXChannelEnabled();

  // Enables Y and Z channels outputs
  void enableYZChannels();

  // Disables Y and Z channels outputs
  void disableYZChannels();

  // Checks if YZ channels outputs are enabled
  bool areYZChannelsEnabled();

  // Sets decimation filter bandwidth. Allowed values are 800, 400, 200 or 100. Defaults to 100 on invalid values.
  void setFilterBandwidth(uint16_t bandwidth);

  // Gets current decimation filter bandwith. Values are in Hz.
  uint16_t getFilterBandwith();

  // Enables continuous mode. Continuous mode frequency must be greater than 0.
  void enableContinuousMode();

  // Disables continuous mode.
  void disableContinuousMode();

  // Checks if continuous mode is enabled.
  bool isContinuousModeEnabled();

  // Sets continuous mode frequency. Allowed values are 1000, 200, 100, 50, 20, 10, 1 and 0 (off). Defaults to 0 (off).
  void setContinuousModeFrequency(uint16_t frequency);

  // Gets continuous mode frequency.
  uint16_t getContinuousModeFrequency();

  // Enables periodic set
  void enablePeriodicSet();

  // Disables periodic set
  void disablePeriodicSet();

  // Checks if periodic set is enabled
  bool isPeriodicSetEnabled();  

  // Sets how often the chip will perform an automatic set operation. Allowed values are 1, 25, 75, 100, 250, 500, 1000, 2000. Defaults to 1.
  void setPeriodicSetSamples(uint16_t numberOfSamples);

  // Gets how many times the chip is performing an automatic set operation.
  uint16_t getPeriodicSetSamples();

  // Apply extra current from positive side to negative side of the coil. This feature can be used to check whether the sensor has been saturated.
  void applyExtraCurrentPosToNeg();

  // Remove extra current from positive side to negative side of the coil.
  void removeExtraCurrentPosToNeg();

  // Checks if extra current is applied from positive to negative side of coil.
  bool isExtraCurrentAppliedPosToNeg();

  // Apply extra current from negative side to positive side of the coil. This feature can be used to check whether the sensor has been saturated.
  void applyExtracurrentNegToPos();

  // Remove extra current from negative side to positive side of the coil.
  void removeExtracurrentNegToPos();

  // Checks if extra current is applied from negative to positive side of coil.
  bool isExtraCurrentAppliedNegToPos();

  // Get X axis measurement
  uint32_t getMeasurementX();

  // Get Y axis measurement
  uint32_t getMeasurementY();

  // Get Z axis measurement
  uint32_t getMeasurementZ();
};

#endif