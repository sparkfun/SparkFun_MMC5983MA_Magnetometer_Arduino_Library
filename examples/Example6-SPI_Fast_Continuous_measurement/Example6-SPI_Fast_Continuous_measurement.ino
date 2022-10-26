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

int interruptPin = 2;

volatile bool newDataAvailable = true;
uint32_t rawValueX = 0;
uint32_t rawValueY = 0;
uint32_t rawValueZ = 0;
double normalizedX = 0;
double normalizedY = 0;
double normalizedZ = 0;
double heading = 0;

void setup()
{
    Serial.begin(115200);
    Serial.println("MMC5983MA Example");

    SPI.begin();

    // Configure the interrupt pin for the "Measurement Done" interrupt
    pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), interruptRoutine, RISING);

    if (myMag.begin(csPin) == false)
    {
        Serial.println("MMC5983MA did not respond - check your wiring. Freezing.");
        while (true)
            ;
    }

    myMag.softReset();

    Serial.println("MMC5983MA connected");

    Serial.println("Setting filter bandwith to 800 Hz for continuous operation...");
    myMag.setFilterBandwidth(800);
    Serial.print("Reading back filter bandwith: ");
    Serial.println(myMag.getFilterBandwith());

    Serial.println("Setting continuous measurement frequency to 100 Hz...");
    myMag.setContinuousModeFrequency(100);
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
    }
}

void interruptRoutine()
{
    newDataAvailable = true;
}
