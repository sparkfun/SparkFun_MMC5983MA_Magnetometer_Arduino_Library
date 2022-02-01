/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034
  
  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file declares all functions used in the MMC5983MA High Performance Magnetometer Arduino Library IO layer.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _SPARKFUN_MMC5983MA_IO_
#define _SPARKFUN_MMC5983MA_IO_

#include <Arduino.h>
#include <Wire.h>

class SFE_MMC5983MA_IO
{
private:
    TwoWire *_i2cPort;
    uint8_t _address;

public:
    // Default constructor.
    SFE_MMC5983MA_IO(){};

    // Default destructor
    ~SFE_MMC5983MA_IO(){};

    // Starts two wire interface.
    bool begin(const uint8_t address, TwoWire &wirePort);

    // Returns true if we get a reply from the I2C device.
    bool isConnected();

    // Read a single uint8_t from a register.
    uint8_t readSingleByte(const uint8_t registerAddress);

    // Writes a single uint8_t into a register.
    void writeSingleByte(const uint8_t registerAddress, const uint8_t value);

    // Reads multiple bytes from a register into buffer uint8_t array.
    void readMultipleBytes(const uint8_t registerAddress, uint8_t* const buffer , const uint8_t packetLength);

    // Writes multiple bytes to register from buffer uint8_t array.
    void writeMultipleBytes(const uint8_t registerAddress, uint8_t* const buffer, const uint8_t packetLength);

    // Sets a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
    void setRegisterBit(const uint8_t registerAddress, const uint8_t bitMask);

    // Clears a single bit in a specific register. Bit position ranges from 0 (lsb) to 7 (msb).
    void clearRegisterBit(const uint8_t registerAddress, const uint8_t bitMask);

    // Returns true if a specific bit is set in a register. Bit position ranges from 0 (lsb) to 7 (msb).
    bool isBitSet(const uint8_t  registerAddress, const uint8_t bitMask);
};

#endif