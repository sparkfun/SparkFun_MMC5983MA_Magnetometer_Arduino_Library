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

volatile bool newDataAvailable = true;
uint32_t rawValueX = 0;
uint32_t rawValueY = 0;
uint32_t rawValueZ = 0;
double scaledX = 0;
double scaledY = 0;
double scaledZ = 0;
double heading = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("MMC5983MA Example");

    Wire.begin();

    // Configure the interrupt pin for the "Measurement Done" interrupt
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interruptRoutine, RISING);

    if (myMag.begin() == false)
    {
        Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
        while (true)
            ;
    }

    myMag.softReset();

    Serial.println("MMC5983MA connected");

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

    Serial.println("Enabling interrupt...");
    myMag.enableInterrupt();
    Serial.print("Reading back interrupt status: ");
    Serial.println(myMag.isInterruptEnabled() ? "enabled" : "disabled");

    // Set our interrupt flag, just in case we missed the rising edge
    newDataAvailable = true;
}

void loop()
{
    if (newDataAvailable == true)
    {
        newDataAvailable = false; // Clear our interrupt flag
        myMag.clearMeasDoneInterrupt(); // Clear the MMC5983 interrupt

        // Read all three channels simultaneously
        // Note: we are calling readFieldsXYZ to read the fields, not getMeasurementXYZ
        // The measurement is already complete, we do not need to start a new one
        myMag.readFieldsXYZ(&rawValueX, &rawValueY, &rawValueZ);
    
        // The magnetic field values are 18-bit unsigned. The _approximate_ zero (mid) point is 2^17 (131072).
        // Here we scale each field to +/- 1.0 to make it easier to calculate an approximate heading.
        //
        // Please note: to properly correct and calibrate the X, Y and Z channels, you need to determine true
        // offsets (zero points) and scale factors (gains) for all three channels. Futher details can be found at:
        // https://thecavepearlproject.org/2015/05/22/calibrating-any-compass-or-accelerometer-for-arduino/
        scaledX = (double)rawValueX - 131072.0;
        scaledX /= 131072.0;
    
        scaledY = (double)rawValueY - 131072.0;
        scaledY /= 131072.0;
    
        scaledZ = (double)rawValueZ - 131072.0;
        scaledZ /= 131072.0;
    
        // Magnetic north is oriented with the Y axis
        // Convert the X and Y fields into heading using atan2 (Arc Tangent 2)
        heading = atan2(scaledX, 0 - scaledY);
    
        // atan2 returns a value between +PI and -PI
        // Convert to degrees
        heading /= PI;
        heading *= 180;
        heading += 180;
    
        Serial.print("Heading: ");
        Serial.println(heading, 1);
    }
}

void interruptRoutine()
{
    newDataAvailable = true;
}
