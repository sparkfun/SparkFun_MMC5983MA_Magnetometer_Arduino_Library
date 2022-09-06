/*
  This is a library written for the MMC5983MA High Performance Magnetometer.
  SparkFun sells these at its website:
  https://www.sparkfun.com/products/19034

  Do you like this library? Help support open source hardware. Buy a board!

  Written by Ricardo Ramos  @ SparkFun Electronics, February 2nd, 2022.
  This file implements all functions used in the MMC5983MA High Performance Magnetometer Arduino Library.

  SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).
  See LICENSE.md for more information.
*/

#include "SparkFun_MMC5983MA_Arduino_Library.h"
#include "SparkFun_MMC5983MA_Arduino_Library_Constants.h"

void SFE_MMC5983MA::setShadowBit(uint8_t registerAddress, const uint8_t bitMask)
{
    uint8_t *shadowRegister = nullptr;

    // Which register are we referring to?
    switch (registerAddress)
    {
    case INT_CTRL_0_REG:
    {
        shadowRegister = &memoryShadow.internalControl0;
    }
    break;

    case INT_CTRL_1_REG:
    {
        shadowRegister = &memoryShadow.internalControl1;
    }
    break;

    case INT_CTRL_2_REG:
    {
        shadowRegister = &memoryShadow.internalControl2;
    }
    break;

    case INT_CTRL_3_REG:
    {
        shadowRegister = &memoryShadow.internalControl3;
    }
    break;

    default:
        break;
    }

    if (shadowRegister)
    {
        *shadowRegister |= bitMask;
        mmc_io.writeSingleByte(registerAddress, *shadowRegister);
    }
}

void SFE_MMC5983MA::clearShadowBit(uint8_t registerAddress, const uint8_t bitMask)
{
    uint8_t *shadowRegister = nullptr;

    // Which register are we referring to?
    switch (registerAddress)
    {
    case INT_CTRL_0_REG:
    {
        shadowRegister = &memoryShadow.internalControl0;
    }
    break;

    case INT_CTRL_1_REG:
    {
        shadowRegister = &memoryShadow.internalControl1;
    }
    break;

    case INT_CTRL_2_REG:
    {
        shadowRegister = &memoryShadow.internalControl2;
    }
    break;

    case INT_CTRL_3_REG:
    {
        shadowRegister = &memoryShadow.internalControl3;
    }
    break;

    default:
        break;
    }

    if (shadowRegister)
    {
        *shadowRegister &= ~bitMask;
        mmc_io.writeSingleByte(registerAddress, *shadowRegister);
    }
}

bool SFE_MMC5983MA::isShadowBitSet(uint8_t registerAddress, const uint8_t bitMask)
{
    // Which register are we referring to?
    switch (registerAddress)
    {
    case INT_CTRL_0_REG:
    {
        return (memoryShadow.internalControl0 & bitMask);
    }
    break;

    case INT_CTRL_1_REG:
    {
        return (memoryShadow.internalControl1 & bitMask);
    }
    break;

    case INT_CTRL_2_REG:
    {
        return (memoryShadow.internalControl2 & bitMask);
    }
    break;

    case INT_CTRL_3_REG:
    {
        return (memoryShadow.internalControl3 & bitMask);
    }
    break;

    default:
        break;
    }

    return false;
}

void SFE_MMC5983MA::setErrorCallback(void (*_errorCallback)(SF_MMC5983MA_ERROR errorCode))
{
    errorCallback = _errorCallback;
}

bool SFE_MMC5983MA::begin(TwoWire &wirePort)
{
    // Initializes I2C and check if device responds
    bool success = mmc_io.begin(wirePort);

    if (!success)
    {
        SAFE_CALLBACK(errorCallback, SF_MMC5983MA_ERROR::I2C_INITIALIZATION_ERROR);
        return false;
    }
    return isConnected();
}

bool SFE_MMC5983MA::begin(uint8_t userCSPin, SPIClass &spiPort)
{
    bool success = mmc_io.begin(userCSPin, spiPort);
    if (!success)
    {
        SAFE_CALLBACK(errorCallback, SF_MMC5983MA_ERROR::SPI_INITIALIZATION_ERROR);
        return false;
    }
    return isConnected();
}

bool SFE_MMC5983MA::begin(uint8_t userCSPin, SPISettings userSettings, SPIClass &spiPort)
{
    bool success = mmc_io.begin(userCSPin, userSettings, spiPort);
    if (!success)
    {
        SAFE_CALLBACK(errorCallback, SF_MMC5983MA_ERROR::SPI_INITIALIZATION_ERROR);
        return false;
    }
    return isConnected();
}

bool SFE_MMC5983MA::isConnected()
{
    // Poll device for its ID.
    uint8_t response;
    response = mmc_io.readSingleByte(PROD_ID_REG);

    if (response != PROD_ID)
    {
        SAFE_CALLBACK(errorCallback, SF_MMC5983MA_ERROR::INVALID_DEVICE);
        return false;
    }
    return true;
}

int SFE_MMC5983MA::getTemperature()
{
    // Send command to device. Since TM_T clears itself we don't need to
    // use the shadow register for this - we can send the command directly to the IC.
    mmc_io.setRegisterBit(INT_CTRL_0_REG, TM_T);

    // Wait until measurement is completed
    do
    {
        // Wait a little so we won't flood MMC with requests
        delay(5);
    } while (!mmc_io.isBitSet(STATUS_REG, MEAS_T_DONE));

    // Get raw temperature value from the IC.
    uint8_t result = mmc_io.readSingleByte(T_OUT_REG);

    // Convert it using the equation provided in the datasheet
    float temperature = -75.0f + (static_cast<float>(result) * (200.0f / 255.0f));

    // Return the integer part of the temperature.
    return static_cast<int>(temperature);
}

void SFE_MMC5983MA::softReset()
{
    // Since SW_RST bit clears itself we don't need to to through the shadow
    // register for this - we can send the command directly to the IC.
    mmc_io.setRegisterBit(INT_CTRL_1_REG, SW_RST);

    // The reset time is 10 msec. but we'll wait 15 msec. just in case.
    delay(15);
}

void SFE_MMC5983MA::enableInterrupt()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if interrupts are enabled using isInterruptEnabled()
    setShadowBit(INT_CTRL_0_REG, INT_MEAS_DONE_EN);
}

void SFE_MMC5983MA::disableInterrupt()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if interrupts are enabled using isInterruptEnabled()
    clearShadowBit(INT_CTRL_0_REG, INT_MEAS_DONE_EN);
}

bool SFE_MMC5983MA::isInterruptEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_0_REG register.
    return isShadowBitSet(INT_CTRL_0_REG, INT_MEAS_DONE_EN);
}

void SFE_MMC5983MA::enable3WireSPI()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if SPI is enabled using isSPIEnabled()
    setShadowBit(INT_CTRL_3_REG, SPI_3W);
}

void SFE_MMC5983MA::disable3WireSPI()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if is is enabled using isSPIEnabled()
    clearShadowBit(INT_CTRL_3_REG, SPI_3W);
}

bool SFE_MMC5983MA::is3WireSPIEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_3_REG register.
    return isShadowBitSet(INT_CTRL_3_REG, SPI_3W);
}

void SFE_MMC5983MA::performSetOperation()
{
    // Since SET bit clears itself we don't need to to through the shadow
    // register for this - we can send the command directly to the IC.
    mmc_io.setRegisterBit(INT_CTRL_0_REG, SET_OPERATION);

    // Wait until bit clears itself.
    delay(1);
}

void SFE_MMC5983MA::performResetOperation()
{
    // Since RESET bit clears itself we don't need to to through the shadow
    // register for this - we can send the command directly to the IC.
    mmc_io.setRegisterBit(INT_CTRL_0_REG, RESET_OPERATION);

    // Wait until bit clears itself.
    delay(1);
}

void SFE_MMC5983MA::enableAutomaticSetReset()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if automatic set/reset is enabled using isAutomaticSetResetEnabled()
    setShadowBit(INT_CTRL_0_REG, AUTO_SR_EN);
}

void SFE_MMC5983MA::disableAutomaticSetReset()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if automatic set/reset is enabled using isAutomaticSetResetEnabled()
    clearShadowBit(INT_CTRL_0_REG, AUTO_SR_EN);
}

bool SFE_MMC5983MA::isAutomaticSetResetEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_0_REG register.
    return isShadowBitSet(INT_CTRL_0_REG, AUTO_SR_EN);
}

void SFE_MMC5983MA::enableXChannel()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if the channel is enabled using isXChannelEnabled()
    // and since it's a inhibit bit it must be cleared so X channel will
    // be enabled.
    clearShadowBit(INT_CTRL_1_REG, X_INHIBIT);
}

void SFE_MMC5983MA::disableXChannel()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if the channel is enabled using isXChannelEnabled()
    // and since it's a inhibit bit it must be set so X channel will
    // be disabled.
    setShadowBit(INT_CTRL_1_REG, X_INHIBIT);
}

bool SFE_MMC5983MA::isXChannelEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_1_REG register.
    return isShadowBitSet(INT_CTRL_1_REG, X_INHIBIT);
}

void SFE_MMC5983MA::enableYZChannels()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if channels are enabled using areYZChannelsEnabled()
    // and since it's a inhibit bit it must be cleared so X channel will
    // be enabled.
    clearShadowBit(INT_CTRL_1_REG, YZ_INHIBIT);
}

void SFE_MMC5983MA::disableYZChannels()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if channels are enabled using areYZChannelsEnabled()
    // and since it's a inhibit bit it must be cleared so X channel will
    // be disabled.
    setShadowBit(INT_CTRL_1_REG, YZ_INHIBIT);
}

bool SFE_MMC5983MA::areYZChannelsEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_1_REG register.
    return isShadowBitSet(INT_CTRL_1_REG, YZ_INHIBIT);
}

void SFE_MMC5983MA::setFilterBandwidth(uint16_t bandwidth)
{
    // These must be set/cleared using the shadow memory since it can be read
    // using getFilterBandwith()
    switch (bandwidth)
    {
    case 800:
    {
        setShadowBit(INT_CTRL_1_REG, BW0);
        setShadowBit(INT_CTRL_1_REG, BW1);
    }
    break;

    case 400:
    {
        clearShadowBit(INT_CTRL_1_REG, BW0);
        setShadowBit(INT_CTRL_1_REG, BW1);
    }
    break;

    case 200:
    {
        setShadowBit(INT_CTRL_1_REG, BW0);
        clearShadowBit(INT_CTRL_1_REG, BW1);
    }
    break;

    case 100:
    default:
    {
        clearShadowBit(INT_CTRL_1_REG, BW0);
        clearShadowBit(INT_CTRL_1_REG, BW1);
    }
    break;
    }
}

uint16_t SFE_MMC5983MA::getFilterBandwith()
{
    bool bw0 = isShadowBitSet(INT_CTRL_1_REG, BW0);
    bool bw1 = isShadowBitSet(INT_CTRL_1_REG, BW1);

    uint8_t value = (bw1 ? 2 : 0) + (bw0 ? 1 : 0);
    uint16_t retVal = 0;
    switch (value)
    {
    case 1:
        retVal = 200;
        break;

    case 2:
        retVal = 400;
        break;

    case 3:
        retVal = 800;
        break;

    default:
    case 0:
        retVal = 100;
        break;
    }

    return retVal;
}

void SFE_MMC5983MA::enableContinuousMode()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if continuous mode is enabled using isContinuousModeEnabled()
    setShadowBit(INT_CTRL_2_REG, CMM_EN);
}

void SFE_MMC5983MA::disableContinuousMode()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if continuous mode is enabled using isContinuousModeEnabled()
    clearShadowBit(INT_CTRL_2_REG, CMM_EN);
}

bool SFE_MMC5983MA::isContinuousModeEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_2_REG register.
    return isShadowBitSet(INT_CTRL_2_REG, CMM_EN);
}

void SFE_MMC5983MA::setContinuousModeFrequency(uint16_t frequency)
{
    // These must be set/cleared using the shadow memory since it can be read
    // using getContinuousModeFrequency()
    switch (frequency)
    {
    case 1:
    {
        // CM_FREQ[2:0] = 001
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 10:
    {
        // CM_FREQ[2:0] = 010
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 20:
    {
        // CM_FREQ[2:0] = 011
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 50:
    {
        // CM_FREQ[2:0] = 100
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 100:
    {
        // CM_FREQ[2:0] = 101
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 200:
    {
        // CM_FREQ[2:0] = 110
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 1000:
    {
        // CM_FREQ[2:0] = 111
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        setShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;

    case 0:
    default:
    {
        // CM_FREQ[2:0] = 000
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_2);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_1);
        clearShadowBit(INT_CTRL_2_REG, CM_FREQ_0);
    }
    break;
    }
}

uint16_t SFE_MMC5983MA::getContinuousModeFrequency()
{
    // Since we cannot read INT_CTRL_2_REG we evaluate the shadow
    // memory contents and return the corresponding frequency.

    // Remove unwanted bits
    uint8_t registerValue = memoryShadow.internalControl2 & 0x07;
    uint16_t frequency = 0;

    switch (registerValue)
    {
    case 0x01:
    {
        frequency = 1;
    }
    break;

    case 0x02:
    {
        frequency = 10;
    }
    break;

    case 0x03:
    {
        frequency = 20;
    }
    break;

    case 0x04:
    {
        frequency = 50;
    }
    break;

    case 0x05:
    {
        frequency = 100;
    }
    break;

    case 0x06:
    {
        frequency = 200;
    }
    break;

    case 0x07:
    {
        frequency = 1000;
    }
    break;

    case 0:
    default:
        break;
    }

    return frequency;
}

void SFE_MMC5983MA::enablePeriodicSet()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if periodic set is enabled using isContinuousModeEnabled()
    setShadowBit(INT_CTRL_2_REG, EN_PRD_SET);
}

void SFE_MMC5983MA::disablePeriodicSet()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if periodic set is enabled using isContinuousModeEnabled()
    clearShadowBit(INT_CTRL_2_REG, EN_PRD_SET);
}

bool SFE_MMC5983MA::isPeriodicSetEnabled()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_2_REG register.
    return isShadowBitSet(INT_CTRL_2_REG, EN_PRD_SET);
}

void SFE_MMC5983MA::setPeriodicSetSamples(const uint16_t numberOfSamples)
{
    // We must use the shadow memory to do all bits manipulations but
    // we need to access the shadow memory directly, change bits and
    // write back at once.
    switch (numberOfSamples)
    {
    case 25:
    {
        // PRD_SET[2:0] = 001
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 75:
    {
        // PRD_SET[2:0] = 010
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 100:
    {
        // PRD_SET[2:0] = 011
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 250:
    {
        // PRD_SET[2:0] = 100
        setShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 500:
    {
        // PRD_SET[2:0] = 101
        setShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 1000:
    {
        // PRD_SET[2:0] = 110
        setShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 2000:
    {
        // PRD_SET[2:0] = 111
        setShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        setShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;

    case 1:
    default:
    {
        // PRD_SET[2:0] = 000
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_2);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_1);
        clearShadowBit(INT_CTRL_2_REG, PRD_SET_0);
    }
    break;
    }
}

uint16_t SFE_MMC5983MA::getPeriodicSetSamples()
{

    // Since we cannot read INT_CTRL_2_REG we evaluate the shadow
    // memory contents and return the corresponding period.

    // Remove unwanted bits
    uint8_t registerValue = memoryShadow.internalControl2 & 0x70;
    uint16_t period = 1;

    switch (registerValue)
    {
    case 0x10:
    {
        period = 25;
    }
    break;

    case 0x20:
    {
        period = 75;
    }
    break;

    case 0x30:
    {
        period = 100;
    }
    break;

    case 0x40:
    {
        period = 250;
    }
    break;

    case 0x50:
    {
        period = 500;
    }
    break;

    case 0x60:
    {
        period = 1000;
    }
    break;

    case 0x70:
    {
        period = 2000;
    }
    break;

    case 0x0:
    default:
        break;
    }

    return period;
}

void SFE_MMC5983MA::applyExtraCurrentPosToNeg()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if extra current is applied using isExtraCurrentAppliedPosToNeg()
    setShadowBit(INT_CTRL_3_REG, ST_ENP);
}

void SFE_MMC5983MA::removeExtraCurrentPosToNeg()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if extra current is applied using isExtraCurrentAppliedPosToNeg()
    clearShadowBit(INT_CTRL_3_REG, ST_ENP);
}

bool SFE_MMC5983MA::isExtraCurrentAppliedPosToNeg()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_3_REG register.
    return isShadowBitSet(INT_CTRL_3_REG, ST_ENP);
}

void SFE_MMC5983MA::applyExtracurrentNegToPos()
{
    // This bit must be set through the shadow memory or we won't be
    // able to check if extra current is applied using isExtraCurrentAppliedNegToPos()
    setShadowBit(INT_CTRL_3_REG, ST_ENM);
}

void SFE_MMC5983MA::removeExtracurrentNegToPos()
{
    // This bit must be cleared through the shadow memory or we won't be
    // able to check if extra current is applied using isExtraCurrentAppliedNegToPos()
    clearShadowBit(INT_CTRL_3_REG, ST_ENM);
}

bool SFE_MMC5983MA::isExtraCurrentAppliedNegToPos()
{
    // Get the bit value from the shadow register since the IC does not
    // allow reading INT_CTRL_3_REG register.
    return isShadowBitSet(INT_CTRL_3_REG, ST_ENM);
}

uint32_t SFE_MMC5983MA::getMeasurementX()
{
    // Send command to device. TM_M self clears so we can access it directly.
    mmc_io.setRegisterBit(INT_CTRL_0_REG, TM_M);

    // Wait until measurement is completed
    do
    {
        // Wait a little so we won't flood MMC with requests
        delay(5);
    } while (!mmc_io.isBitSet(STATUS_REG, MEAS_M_DONE));

    uint32_t temp = 0;
    uint32_t result = 0;
    uint8_t buffer[7] = {0};

    mmc_io.readMultipleBytes(X_OUT_0_REG, buffer, 7);

    temp = static_cast<uint32_t>(buffer[X_OUT_0_REG]);
    temp = temp << XYZ_0_SHIFT;
    result |= temp;

    temp = static_cast<uint32_t>(buffer[X_OUT_1_REG]);
    temp = temp << XYZ_1_SHIFT;
    result |= temp;

    temp = static_cast<uint32_t>(buffer[XYZ_OUT_2_REG]);
    temp &= X2_MASK;
    temp = temp >> 6;
    result |= temp;
    return result;
}

uint32_t SFE_MMC5983MA::getMeasurementY()
{
    // Send command to device. TM_M self clears so we can access it directly.
    mmc_io.setRegisterBit(INT_CTRL_0_REG, TM_M);

    // Wait until measurement is completed
    do
    {
        // Wait a little so we won't flood MMC with requests
        delay(5);
    } while (!mmc_io.isBitSet(STATUS_REG, MEAS_M_DONE));

    uint32_t temp = 0;
    uint32_t result = 0;
    uint8_t registerValue = 0;

    registerValue = (mmc_io.readSingleByte(Y_OUT_0_REG));

    temp = static_cast<uint32_t>(registerValue);
    temp = temp << XYZ_0_SHIFT;
    result |= temp;

    registerValue = (mmc_io.readSingleByte(Y_OUT_1_REG));

    temp = static_cast<uint32_t>(registerValue);
    temp = temp << XYZ_1_SHIFT;
    result |= temp;

    registerValue = (mmc_io.readSingleByte(XYZ_OUT_2_REG));
    temp = static_cast<uint32_t>(registerValue);
    temp &= Y2_MASK;
    temp = temp >> 4;
    result |= temp;
    return result;
}

uint32_t SFE_MMC5983MA::getMeasurementZ()
{
    // Send command to device. TM_M self clears so we can access it directly.
    mmc_io.setRegisterBit(INT_CTRL_0_REG, TM_M);

    // Wait until measurement is completed
    do
    {
        // Wait a little so we won't flood MMC with requests
        delay(5);
    } while (!mmc_io.isBitSet(STATUS_REG, MEAS_M_DONE));

    uint32_t temp = 0;
    uint32_t result = 0;
    uint8_t registerValue = 0;

    registerValue = (mmc_io.readSingleByte(Z_OUT_0_REG));

    temp = static_cast<uint32_t>(registerValue);
    temp = temp << XYZ_0_SHIFT;
    result |= temp;

    registerValue = (mmc_io.readSingleByte(Z_OUT_1_REG));

    temp = static_cast<uint32_t>(registerValue);
    temp = temp << XYZ_1_SHIFT;
    result |= temp;

    registerValue = (mmc_io.readSingleByte(XYZ_OUT_2_REG));

    temp = static_cast<uint32_t>(registerValue);
    temp &= Z2_MASK;
    temp = temp >> 2;
    result |= temp;
    return result;
}
