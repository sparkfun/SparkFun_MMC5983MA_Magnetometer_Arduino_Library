/*
  Compute magnetic heading from the MMC5983MA
  By: Nathan Seidle and Ricardo Ramos
  SparkFun Electronics
  Date: April 14th, 2022
  License: SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/19034

  This example demonstrates how to compute the heading based on the basic X/Y/Z readings from the sensor over Qwiic

  Hardware Connections:
  Plug a Qwiic cable into the sensor and a RedBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper
  (https://www.sparkfun.com/products/17912) Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h>

#include <SparkFun_MMC5983MA_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_MMC5983MA

SFE_MMC5983MA myMag;

void setup()
{
    Serial.begin(115200);
    Serial.println("MMC5983MA Example");

    Wire.begin();

    if (myMag.begin() == false)
    {
        Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
        while (true)
            ;
    }

    myMag.softReset();

    Serial.println("MMC5983MA connected");
}

void loop()
{
    uint32_t rawValueX = 0;
    uint32_t rawValueY = 0;
    uint32_t rawValueZ = 0;
    double normalizedX = 0;
    double normalizedY = 0;
    double normalizedZ = 0;
    double heading = 0;

    // Read all three channels simultaneously
    myMag.getMeasurementXYZ(&rawValueX, &rawValueY, &rawValueZ);

    // The magnetic field values are 18-bit unsigned. The zero (mid) point is 2^17 (131072).
    // Normalize each field to +/- 1.0
    normalizedX = (double)rawValueX - 131072.0;
    normalizedX /= 131072.0;

    normalizedY = (double)rawValueY - 131072.0;
    normalizedY /= 131072.0;

    normalizedZ = (double)rawValueZ - 131072.0;
    normalizedZ /= 131072.0;

    // Magnetic north is oriented with the Y axis
    // Convert the X and Y fields into heading using atan2 (Arc Tangent 2)
    heading = atan2(normalizedX, 0 - normalizedY);

    // atan2 returns a value between +PI and -PI
    // Convert to degrees
    heading /= PI;
    heading *= 180;
    heading += 180;

    Serial.print("Heading: ");
    Serial.println(heading, 1);
    delay(100);
}
