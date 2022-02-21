#include <Arduino.h>
#include "Wire.h"
#include "SparkFun_MMC5983MA_Arduino_Library.h"

#define interruptPin 2
volatile bool dataValid = false;
uint32_t rawValue = 0;
double heading = 0;
double normalizedX = 0;
double normalizedY = 0;
double normalizedZ = 0;

SFE_MMC5983MA myMag;

void setup()
{
    Serial.begin(115200);
    Wire.begin();
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interruptRoutine, FALLING);

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
        Serial.println(myMag.isAutomaticSetResetEnabled() ? "enabled." : "disabled.");

        Serial.println("Enabling continuous mode...");
        myMag.enableContinuousMode();
        Serial.print("Reading back continuous mode: ");
        Serial.println(myMag.isContinuousModeEnabled() ? "enabled." : "disabled.");
    }
    else
    {
        Serial.println("MMC5983MA did not respond - check your wiring.");
        Serial.println("System halted.");
        while (1)
        {
        }
    }

    myMag.enableInterrupt();
}

void loop()
{
    do
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

        dataValid = false;
        myMag.enableInterrupt();
    } while (dataValid == true);
}

void interruptRoutine()
{
    dataValid = true;
}