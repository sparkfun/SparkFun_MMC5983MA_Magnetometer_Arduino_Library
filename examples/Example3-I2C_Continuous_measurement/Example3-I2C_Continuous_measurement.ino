/*
  Fast, interrupt driven heading calculation using the MMC5983MA magnetometer.
  By: Nathan Seidle and Ricardo Ramos
  SparkFun Electronics
  Date: April 14th, 2022
  License: SparkFun code, firmware, and software is released under the MIT License(http://opensource.org/licenses/MIT).

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/19034

  This example demonstrates how to use interrupts to quickly read the sensor instead of polling.

  Hardware Connections:
  Solder a wire from the INT pin and wire it to pin 2 on a RedBoard
  Plug a Qwiic cable into the sensor and a RedBoard
  If you don't have a platform with a Qwiic connection use the SparkFun Qwiic Breadboard Jumper
  (https://www.sparkfun.com/products/17912) Open the serial monitor at 115200 baud to see the output
*/

#include <Wire.h>

#include <SparkFun_MMC5983MA_Arduino_Library.h> //Click here to get the library: http://librarymanager/All#SparkFun_MMC5983MA

SFE_MMC5983MA myMag;

int interruptPin = 2;

volatile bool newDataAvailable = false;
unsigned int rawValue = 0;
double heading = 0;
double normalizedX = 0;
double normalizedY = 0;
double normalizedZ = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("MMC5983MA Example");

    Wire.begin();

    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interruptRoutine, FALLING);

    if (myMag.begin() == false)
    {
        Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
        while (true)
            ;
    }

    myMag.softReset();

    Serial.println("MMC5983MA connected");

    Serial.print("Die temperature: ");
    int celsius = myMag.getTemperature();
    Serial.print(celsius);
    Serial.print("°C or ");
    float fahrenheit = (celsius * 9.0f / 5.0f) + 32.0f;
    Serial.print((int)fahrenheit);
    Serial.println("°F");

    Serial.println("Setting filter bandwith to 100 Hz for continuous operation...");
    myMag.setFilterBandwidth(100);
    Serial.print("Reading back filter bandwith: ");
    Serial.println(myMag.getFilterBandwith());

    Serial.println("Setting continuous measurement frequency to 10 Hz...");
    myMag.setContinuousModeFrequency(10);
    Serial.print("Reading back continuous measurement frequency: ");
    Serial.println(myMag.getContinuousModeFrequency());

    Serial.println("Enabling auto set/reset...");
    myMag.enableAutomaticSetReset();
    Serial.print("Reading back automatic set/reset: ");
    Serial.println(myMag.isAutomaticSetResetEnabled() ? "enabled" : "disabled");

    Serial.println("Enabling continuous mode...");
    myMag.enableContinuousMode();
    Serial.print("Reading back continuous mode: ");
    Serial.println(myMag.isContinuousModeEnabled() ? "enabled" : "disabled");

    myMag.enableInterrupt();
}

void loop()
{
    if (newDataAvailable == true)
    {
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

        newDataAvailable = false;
        myMag.enableInterrupt();
    }
}

void interruptRoutine()
{
    newDataAvailable = true;
}