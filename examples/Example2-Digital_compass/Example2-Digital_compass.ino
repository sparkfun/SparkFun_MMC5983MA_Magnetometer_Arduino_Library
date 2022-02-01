#include <Arduino.h>
#include "Wire.h"
#include "SparkFun_MMC5983MA_Arduino_Library.h"

SFE_MMC5983MA sfe_mmc5983;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    if (sfe_mmc5983.begin())
    {
        sfe_mmc5983.softReset();
        Serial.println("MMC5983MA connected.");
        Serial.print("Die temperature: ");
        int celsius = sfe_mmc5983.getTemperature();
        Serial.print(celsius);
        Serial.print("°C or ");
        float farenheit = (celsius * 9.0f / 5.0f) + 32.0f;
        Serial.print(static_cast<int>(farenheit));
        Serial.println("°F.");
        Serial.println("Starting measurements in 2 seconds...");
        delay(2000);
    }
    else
    {
        Serial.println("MMC5983MA did not respond - check your wiring.");
        Serial.println("System halted.");
        while (1)
        {
        }
    }
}

void loop()
{
    uint32_t rawValue = 0;
    double heading = 0;
    double normalizedX = 0;
    double normalizedY = 0;
    double normalizedZ = 0;

    rawValue = sfe_mmc5983.getMeasurementX();
    normalizedX = static_cast<double>(rawValue) - 131072.0;
    normalizedX /= 131072.0;

    rawValue = sfe_mmc5983.getMeasurementY();
    normalizedY = static_cast<double>(rawValue) - 131072.0;
    normalizedY /= 131072.0;

    rawValue = sfe_mmc5983.getMeasurementZ();
    normalizedZ = static_cast<double>(rawValue) - 131072.0;
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
    delay(10);
}