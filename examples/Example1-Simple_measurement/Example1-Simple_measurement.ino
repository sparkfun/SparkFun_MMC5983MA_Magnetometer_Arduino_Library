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
    uint32_t currentX = 0;
    uint32_t currentY = 0;
    uint32_t currentZ = 0;
    double normalizedX = 0;
    double normalizedY = 0;
    double normalizedZ = 0;

    currentX = sfe_mmc5983.getMeasurementX();
    currentY = sfe_mmc5983.getMeasurementY();
    currentZ = sfe_mmc5983.getMeasurementZ();

    Serial.print("X axis raw value: ");
    Serial.print(currentX);
    Serial.print("\tY axis raw value: ");
    Serial.print(currentY);
    Serial.print("\tZ axis raw value: ");
    Serial.println(currentZ);

    normalizedX = static_cast<double>(currentX) - 131072.0;
    normalizedX /= 131072.0;
    normalizedY = static_cast<double>(currentY) - 131072.0;
    normalizedY /= 131072.0;
    normalizedZ = static_cast<double>(currentZ) - 131072.0;
    normalizedZ /= 131072.0;

    Serial.print("X axis field (Gauss): ");
    Serial.print(normalizedX * 8, 5);

    Serial.print("\tY axis field (Gauss): ");
    Serial.print(normalizedY * 8, 5);

    Serial.print("\tZ axis field (Gauss): ");
    Serial.println(normalizedZ * 8, 5);

    Serial.println();
    delay(100);
}