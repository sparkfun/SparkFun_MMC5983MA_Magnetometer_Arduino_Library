/*
  X/Y/Z magnetic field and raw readings from the MMC5983MA
  By: Nathan Seidle and Ricardo Ramos
  SparkFun Electronics
  Date: April 14th, 2022
  License: SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/19034

  This example demonstrates how to read the basic X/Y/Z readings from the sensor over SPI

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

    int celsius = myMag.getTemperature();
    float fahrenheit = (celsius * 9.0f / 5.0f) + 32.0f;

    Serial.print("Die temperature: ");
    Serial.print(celsius);
    Serial.print("°C or ");
    Serial.print((int)fahrenheit);
    Serial.println("°F");
}

void loop()
{
    uint32_t currentX = 0;
    uint32_t currentY = 0;
    uint32_t currentZ = 0;
    double normalizedX = 0;
    double normalizedY = 0;
    double normalizedZ = 0;

    // This reads the X, Y and Z channels consecutively
    // (Useful if you have one or more channels disabled)
    currentX = myMag.getMeasurementX();
    currentY = myMag.getMeasurementY();
    currentZ = myMag.getMeasurementZ();

    // Or, we could read all three simultaneously
    //myMag.getMeasurementXYZ(&currentX, &currentY, &currentZ);

    Serial.print("X axis raw value: ");
    Serial.print(currentX);
    Serial.print("\tY axis raw value: ");
    Serial.print(currentY);
    Serial.print("\tZ axis raw value: ");
    Serial.println(currentZ);

    // The magnetic field values are 18-bit unsigned. The zero (mid) point is 2^17 (131072).
    // Normalize each field to +/- 1.0
    normalizedX = (double)currentX - 131072.0;
    normalizedX /= 131072.0;
    normalizedY = (double)currentY - 131072.0;
    normalizedY /= 131072.0;
    normalizedZ = (double)currentZ - 131072.0;
    normalizedZ /= 131072.0;

    // The magnetometer full scale is +/- 8 Gauss
    // Multiply the normalized values by 8 to convert to Gauss
    Serial.print("X axis field (Gauss): ");
    Serial.print(normalizedX * 8, 5); // Print with 5 decimal places

    Serial.print("\tY axis field (Gauss): ");
    Serial.print(normalizedY * 8, 5);

    Serial.print("\tZ axis field (Gauss): ");
    Serial.println(normalizedZ * 8, 5);

    Serial.println();
    delay(100);
}
