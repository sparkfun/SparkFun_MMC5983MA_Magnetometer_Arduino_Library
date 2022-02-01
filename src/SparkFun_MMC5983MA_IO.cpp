/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file implements all functions used in the MMC5983MA High Performance Magnetometer Arduino Library IO layer.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "SparkFun_MMC5983MA_IO.h"
#include "SparkFun_MMC5983MA_Arduino_Library_Constants.h"

bool SFE_MMC5983MA_IO::begin(const uint8_t address, TwoWire &i2cPort)
{
    _address = address;
    _i2cPort = &i2cPort;
    return isConnected();
}

bool SFE_MMC5983MA_IO::isConnected()
{
    _i2cPort->beginTransmission(_address);
    return _i2cPort->endTransmission() == 0;
}

void SFE_MMC5983MA_IO::writeMultipleBytes(const uint8_t registerAddress, uint8_t* const buffer, uint8_t const packetLength)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	for (uint8_t i = 0; i < packetLength; i++) 
		_i2cPort->write(buffer[i]);
	
	_i2cPort->endTransmission();
}

void SFE_MMC5983MA_IO::readMultipleBytes(const uint8_t registerAddress, uint8_t* const buffer, const uint8_t packetLength)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->endTransmission();

	_i2cPort->requestFrom(_address, packetLength);
	for (uint8_t i = 0; (i < packetLength) && _i2cPort->available(); i++)
		buffer[i] = _i2cPort->read();
}

uint8_t SFE_MMC5983MA_IO::readSingleByte(const uint8_t registerAddress)
{
	uint8_t result;
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->endTransmission();
	_i2cPort->requestFrom(_address, 1U);
	result = _i2cPort->read();
	return result;
}

void SFE_MMC5983MA_IO::writeSingleByte(const uint8_t registerAddress, const uint8_t value)
{
	_i2cPort->beginTransmission(_address);
	_i2cPort->write(registerAddress);
	_i2cPort->write(value);
	_i2cPort->endTransmission();
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