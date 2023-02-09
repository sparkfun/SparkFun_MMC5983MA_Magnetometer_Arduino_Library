/*
  Removing the bridge offset from the MMC5983MA
  By: Paul Clark
  SparkFun Electronics
  Date: February 9th, 2023
  License: SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/19034

  This example shows how to remove the sensor (bridge) offset.
  Press a key (send any character) to perform the set-reset and update the offset.

  Hardware Connections:
  Plug a Qwiic cable into the sensor and a RedBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper
  (https://www.sparkfun.com/products/17912) Open the serial monitor at 115200 baud to see the output

  From the datasheet:

  USING SET AND RESET TO REMOVE BRIDGE OFFSET
  
  The integrated SET and RESET functions of the MMC5893VA enables the user to remove the error
  associated with bridge Offset change as a function of temperature, thereby enabling more precise heading
  measurements over a wider temperature than competitive technologies. The SET and RESET
  functions effectively alternately flip the magnetic sensing polarity of the sensing elements of the device.
  
  1) The most accurate magnetic field measurements can be obtained by using the protocol described
  as follows: Perform SET. This sets the internal magnetization of the sensing resistors in the
  direction of the SET field.
  
  2) Perform MEASUREMENT. This measurement will contain not only the sensors response to the
  external magnetic field, H, but also the Offset; in other words,
  Output1 = +H + Offset.
  
  3) Perform RESET. This resets the internal magnetization of the sensing resistors in the
  direction of the RESET field, which is opposite to the SET field (180o opposed).
  
  4) Perform MEASUREMENT. This measurement will contain both the sensors response to the
  external field and also the Offset. In other words,
  Output2 = -H + Offset.
  
  5) Finally, calculate H by subtracting the two measurements and dividing by 2. This procedure
  effectively eliminates the Offset from the measurement and therefore any changes in the
  Offset over temperature.
  
  Note: To calculate and store the offset; add the two measurements and divide by 2. This calculated offset
  value can be subtracted from subsequent measurements to obtain H directly from each measurement.
*/

#include <Wire.h>

#include <SparkFun_MMC5983MA_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_MMC5983MA

SFE_MMC5983MA myMag;

void setup()
{
  Serial.begin(115200);
  
  // Most Serial prints have been commented out - to allow the data to be plotted nicely using Tools \ Serial Plotter
  //Serial.println("MMC5983MA Example");
  
  Wire.begin();
  
  if (myMag.begin() == false)
  {
    Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
    while (true)
      ;
  }
  
  myMag.softReset();
  
  //Serial.println("MMC5983MA connected");
  //Serial.println("Press a key (send any character) to perform the set-reset and update the offset.");
  
  while (Serial.available())
    Serial.read(); // Empty the Serial RX buffer
}

void loop()
{
  // The magnetic field values are 18-bit unsigned. The zero (mid) point is 2^17 (131072).
  // Use static variables to hold the offset. Set to 131072 initially.
  static uint32_t offsetX = 131072;
  static uint32_t offsetY = 131072;
  static uint32_t offsetZ = 131072;

  // Has the user pressed a key?
  if (Serial.available())
  {
    updateOffset(&offsetX, &offsetY, &offsetZ); // Update the offsets

    while (Serial.available())
      Serial.read(); // Empty the Serial RX buffer
  }

  uint32_t currentX = 0;
  uint32_t currentY = 0;
  uint32_t currentZ = 0;

  // This reads the X, Y and Z channels simultaneously
  myMag.getMeasurementXYZ(&currentX, &currentY, &currentZ);

  // The magnetic field values are 18-bit unsigned.
  // The zero (mid) point should be 2^17 (131072).
  // Here we subtract the offset, then normalize each field to +/- 1.0,
  // then multiply by 8 to convert to Gauss
  double normalizedX = (double)currentX - (double)offsetX; // Convert to double _before_ subtracting
  //normalizedX /= 131072.0;
  //normalizedX *= 8.0;
  double normalizedY = (double)currentY - (double)offsetY; // Convert to double _before_ subtracting
  //normalizedY /= 131072.0;
  //normalizedY *= 8.0;
  double normalizedZ = (double)currentZ - (double)offsetZ; // Convert to double _before_ subtracting
  //normalizedZ /= 131072.0;
  //normalizedZ *= 8.0;

  // Print the three channels with commas in between so the Serial Plotter can plot them
  Serial.print(normalizedX, 5); // Print with 5 decimal places
  Serial.print(",");
  Serial.print(normalizedY, 5);
  Serial.print(",");
  Serial.println(normalizedZ, 5);
}

bool updateOffset(uint32_t *offsetX, uint32_t *offsetY, uint32_t *offsetZ) // Update the offsets
{
  bool success = true; // Use AND (&=) to record if any one command fails

  success &= myMag.performSetOperation(); // Perform the SET operation

  uint32_t setX = 131072;
  uint32_t setY = 131072;
  uint32_t setZ = 131072;

  success &= myMag.getMeasurementXYZ(&setX, &setY, &setZ); // Read all three channels
  success &= myMag.getMeasurementXYZ(&setX, &setY, &setZ); // Do it twice - just in case there is noise on the first

  success &= myMag.performResetOperation(); // Perform the RESET operation

  uint32_t resetX = 131072;
  uint32_t resetY = 131072;
  uint32_t resetZ = 131072;

  success &= myMag.getMeasurementXYZ(&resetX, &resetY, &resetZ); // Read all three channels
  success &= myMag.getMeasurementXYZ(&resetX, &resetY, &resetZ); // Do it twice - just in case there is noise on the first

  // Calculate the offset - as per the datasheet.
  // The measurements are 18-bit so it's OK to add them directly.
  *offsetX = (setX + resetX) / 2;
  *offsetY = (setY + resetY) / 2;
  *offsetZ = (setZ + resetZ) / 2;

  return success;
}
