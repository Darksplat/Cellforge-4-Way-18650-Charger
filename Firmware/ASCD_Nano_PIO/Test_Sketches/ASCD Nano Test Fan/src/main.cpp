
/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Copyright 2018 - Under creative commons license 3.0:

Modified by Jeremy Younger @darksplat on 06/12/2025
// https://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
//
// This software is furnished "as is", without technical support, and with no
// warranty, express or implied, as to its usefulness for any purpose.
//
// @brief
// ASDC Nano 4x Arduino Charger / Discharger
// Code for testing the 16x2 LCD 
// Version 2.0.0
//
// @author Email: 
//       Web: www.darksplat.com
*/


#include <Arduino.h>
#include "Hardware.h"   // for FAN_PIN and initFan()

// Guided version of ASCD_Nano_Test_Fan.ino
// - Uses FAN_PIN from Hardware.h (pin 5 on Nano).
// - Ramps PWM from low to max so you can listen/feel the fan response.

constexpr uint8_t START_DUTY = 100;  // roughly where most fans start moving
constexpr uint8_t END_DUTY   = 255;  // full speed
constexpr uint16_t STEP_DELAY_MS = 500;  // time at each speed


void waitForUser(const char* msg)
{
    Serial.println();
    Serial.println(msg);
    Serial.println("Press ENTER in Serial Monitor to continue...");

    while (!Serial.available()) {
        delay(10);
    }
    while (Serial.available()) {
        Serial.read();  // flush
    }
}


void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("ASCD Nano - PWM Fan Guided Test");
    Serial.println("================================");
    Serial.println("This test will ramp the fan speed from low to high using PWM.");
    Serial.println();
    Serial.println("PREPARATION");
    Serial.println("-----------");
    Serial.println("1. Make sure the FAN is connected to the correct header (FAN_PIN).");
    Serial.println("2. Ensure the fan blades can spin freely (nothing blocking them).");
    Serial.println("3. Board should be powered with your normal supply.");
    Serial.println();

    initFan();             // sets FAN_PIN as OUTPUT, LOW
    digitalWrite(FAN_PIN, LOW);

    waitForUser("When ready, place finger near (NOT touching) the fan and listen for speed changes.");
}


void loop()
{
    Serial.println();
    Serial.println("Starting PWM ramp on FAN_PIN...");
    Serial.println("--------------------------------");

    // Small spin-up burst at full speed so you know it's alive
    Serial.println("Spin-up burst (full speed) for 2 seconds...");
    analogWrite(FAN_PIN, 255);
    delay(2000);

    // Now ramp from START_DUTY to END_DUTY
    for (uint8_t duty = START_DUTY; duty <= END_DUTY; duty++) {
        Serial.print("FAN PWM duty: ");
        Serial.print(duty);
        Serial.print(" (approx ");
        Serial.print((duty / 255.0f) * 100.0f, 1);
        Serial.println("%)");

        analogWrite(FAN_PIN, duty);    // set PWM speed
        delay(STEP_DELAY_MS);
    }

    // Hold at full speed for a bit
    Serial.println();
    Serial.println("Holding at full speed for 3 seconds...");
    analogWrite(FAN_PIN, 255);
    delay(3000);

    // Then turn off
    Serial.println("Fan OFF.");
    analogWrite(FAN_PIN, 0);

    Serial.println();
    Serial.println("TEST COMPLETE");
    Serial.println("=============");
    Serial.println("What you should have observed:");
    Serial.println(" - Fan spun up quickly during initial burst.");
    Serial.println(" - Fan speed increased in steps during the ramp.");
    Serial.println(" - No strange noises or stalling.");
    Serial.println();
    Serial.println("If the fan did NOT behave as expected, check wiring and supply.");
    Serial.println("Reset the board to run the test again.");

    // Idle forever; user can reset to repeat
    while (true) {
        delay(1000);
    }
}
