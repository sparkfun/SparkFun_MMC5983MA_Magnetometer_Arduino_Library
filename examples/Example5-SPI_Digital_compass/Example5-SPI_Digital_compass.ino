/*
  Compute magnetic heading over SPI from the MMC5983MA
  By: Nathan Seidle and Ricardo Ramos
  SparkFun Electronics
  Date: April 14th, 2022
  License: SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/19034

  This example demonstrates how to compute a magnetic heading from the sensor over SPI

  Hardware Connections:
  Connect CIPO to MISO, COPI to MOSI, and SCK to SCK, on an Arduino.
  Connect CS to pin 4 on an Arduino.
*/

#include <SPI.h>

#include <SparkFun_MMC5983MA_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_MMC5983MA

SFE_MMC5983MA myMag;

int csPin = 4;

void setup()
{
    Serial.begin(115200);
    Serial.println("MMC5983MA Example");

    SPI.begin();

    if (myMag.begin(csPin) == false)
    {
        Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
        while (true)
            ;
    }

    Serial.println("MMC5983MA connected");

    myMag.softReset();
}

void loop()
{
    unsigned int rawValue = 0;
    double heading = 0;
    double normalizedX = 0;
    double normalizedY = 0;
    double normalizedZ = 0;

    rawValue = myMag.getMeasurementX();
    normalizedX = (double)rawValue - 131072.0;
    normalizedX /= 131072.0;

    rawValue = myMag.getMeasurementY();
    normalizedY = (double)rawValue - 131072.0;
    normalizedY /= 131072.0;

    rawValue = myMag.getMeasurementZ();
    normalizedZ = (double)rawValue - 131072.0;
    normalizedZ /= 131072.0;

    // Magnetic north is oriented with the Y axis
    if (normalizedY != 0)
    {
        if (normalizedX < 0)
        {
            if (normalizedY > 0)
                heading = 57.2958 * atan(-normalizedX / normalizedY); // Quadrant 1
            else
                heading = 57.2958 * atan(-normalizedX / normalizedY) + 180; // Quadrant 2
        }
        else
        {
            if (normalizedY < 0)
                heading = 57.2958 * atan(-normalizedX / normalizedY) + 180; // Quadrant 3
            else
                heading = 360 - (57.2958 * atan(normalizedX / normalizedY)); // Quadrant 4
        }
    }
    else
    {
        // atan of an infinite number is 90 or 270 degrees depending on X value
        if (normalizedX > 0)
            heading = 270;
        else
            heading = 90;
    }

    Serial.print("Heading: ");
    Serial.println(heading, 1);
    delay(100);
}