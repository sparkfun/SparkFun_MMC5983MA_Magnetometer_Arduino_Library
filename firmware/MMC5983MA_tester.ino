// 
// SparkFun MMC5983MA High Performance Magnetometer
//
// Test Procedure: 
// 
// - Load this on a Blackboard or other Arduino-compatible with a Qwiic port
// - Connect a test unit to the Qwiic port
// - LED on pin 13 will light if unit is OK
// - Data output can be checked on UART @ 115200 bps
// - Boards can be changed without the need for resetting the microcontroller
// 

#include <Arduino.h>
#include "Wire.h"
#include "SparkFun_MMC5983MA_Arduino_Library.h"

SFE_MMC5983MA myMag;
#define STATUS_LED LED_BUILTIN

void setup()
{
    Serial.begin(115200);
    Serial.println("MMC5983MA Tester");
    pinMode(STATUS_LED, OUTPUT);
    digitalWrite(STATUS_LED, LOW);
    Wire.begin();
}

void loop()
{
    if (myMag.begin())
    {
        myMag.softReset();
        Serial.println("MMC5983MA connected.");
        Serial.print("Die temperature: ");
        int celsius = myMag.getTemperature();
        Serial.print(celsius);
        Serial.print("°C or ");
        float fahrenheit = (celsius * 9.0f / 5.0f) + 32.0f;
        Serial.print((int)fahrenheit);
        Serial.println("°F.");

        uint32_t currentX = 0;
        uint32_t currentY = 0;
        uint32_t currentZ = 0;
        double normalizedX = 0;
        double normalizedY = 0;
        double normalizedZ = 0;
        double heading = -1;

        currentX = myMag.getMeasurementX();
        currentY = myMag.getMeasurementY();
        currentZ = myMag.getMeasurementZ();

        bool good = true;

        if (currentX == 0 || currentX >= 262143)
            good = false;

        if (currentY == 0 || currentY >= 262143)
            good = false;

        if (currentZ == 0 || currentZ >= 262143)
            good = false;

        if (good)
        {
            normalizedX = (double)currentX - 131072.0;
            normalizedX /= 131072.0;

            normalizedY = (double)currentY - 131072.0;
            normalizedY /= 131072.0;

            normalizedZ = (double)currentZ - 131072.0;
            normalizedZ /= 131072.0;

            Serial.print("X axis raw value: ");
            Serial.print(currentX);
            Serial.print("\tY axis raw value: ");
            Serial.print(currentY);
            Serial.print("\tZ axis raw value: ");
            Serial.println(currentZ);

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
            Serial.println();
            digitalWrite(STATUS_LED, HIGH);
        }
        else
        {
            Serial.println("Values out of range.");
            Serial.println();
            digitalWrite(STATUS_LED, LOW);
        }
        delay(500);
    }
    else
    {
        Serial.println("MMC5983MA not detected.");
        digitalWrite(STATUS_LED, LOW);
        delay(500);
    }
}