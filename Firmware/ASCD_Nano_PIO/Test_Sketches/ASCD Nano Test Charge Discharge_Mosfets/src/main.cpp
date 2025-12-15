
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
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Hardware.h"     // shared pin + mux + shift-register helpers

LiquidCrystal_I2C lcd(0x27, 16, 2);

constexpr uint8_t CYCLE_COUNT = 6;   // how many on/off cycles per test


// -------------------- helper: wait for user --------------------
void waitForUser(const char* line1, const char* line2)
{
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);

    Serial.println();
    Serial.println(line1);
    Serial.println(line2);
    Serial.println("Press ENTER in Serial Monitor to continue...");

    while (!Serial.available()) { delay(10); }
    while (Serial.available())  { Serial.read(); }  // flush
}


// -------------------- MOSFET test function --------------------
void testModule(uint8_t moduleIndex)
{
    uint8_t chargePin    = MODULES[moduleIndex].chargeMosfetShiftIndex;
    uint8_t dischargePin = MODULES[moduleIndex].dischargeMosfetShiftIndex;

    // ------------------------------------------------------------
    // 1. DISCHARGE MOSFET TEST (continuity test)
    // ------------------------------------------------------------
    char header1[17];
    snprintf(header1, sizeof(header1), "Module %u", moduleIndex + 1);

    waitForUser(header1, "Test DISCHG MOSFET");

    Serial.println("------------------------------------------------");
    Serial.print("MODULE "); Serial.print(moduleIndex + 1); Serial.println(" - DISCHARGE MOSFET TEST");
    Serial.println("Multimeter instructions:");
    Serial.println("  Mode: CONTINUITY / DIODE (beep mode)");
    Serial.println("  Black probe: GND");
    Serial.println("  Red probe:   DRAIN of discharge MOSFET");
    Serial.println();
    Serial.println("You should hear a beep ONCE PER SECOND.");
    Serial.println("------------------------------------------------");

    for (uint8_t i = 0; i < CYCLE_COUNT; i++)
    {
        shiftRegisterSetOutput(dischargePin, true);
        delay(1000);

        shiftRegisterSetOutput(dischargePin, false);
        delay(1000);
    }


    // ------------------------------------------------------------
    // 2. CHARGE MOSFET TEST (voltage test)
    // ------------------------------------------------------------
    waitForUser(header1, "Test CHARGE MOSFET");

    Serial.println("------------------------------------------------");
    Serial.print("MODULE "); Serial.print(moduleIndex + 1); Serial.println(" - CHARGE MOSFET TEST");
    Serial.println("Multimeter instructions:");
    Serial.println("  Mode: DC VOLTAGE");
    Serial.println("  Black probe: GND");
    Serial.println("  Red probe:   +VCC pad at TP5100 footprint");
    Serial.println();
    Serial.println("You should see ~12V appear ONCE PER SECOND.");
    Serial.println("------------------------------------------------");

    for (uint8_t i = 0; i < CYCLE_COUNT; i++)
    {
        shiftRegisterSetOutput(chargePin, true);
        delay(1000);

        shiftRegisterSetOutput(chargePin, false);
        delay(1000);
    }

    Serial.println("Module test complete.\n");
}


// -------------------- setup --------------------
void setup()
{
    Serial.begin(115200);

    lcd.init();
    lcd.backlight();

    initShiftRegister();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MOSFET TEST");
    lcd.setCursor(0,1);
    lcd.print("Modules 1-4");
    delay(2000);

    Serial.println("ASCD Nano - Charge/Discharge MOSFET Guided Test");
    Serial.println("================================================");
    Serial.println("You will be prompted before each test.");
    Serial.println("Follow the multimeter instructions carefully.");
    Serial.println("================================================");
}


// -------------------- main loop --------------------
void loop()
{
    for (uint8_t m = 0; m < MODULE_COUNT; m++)
    {
        testModule(m);
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("MOSFET TEST");
    lcd.setCursor(0,1);
    lcd.print("COMPLETE!");

    Serial.println("All MOSFET tests complete.");
    Serial.println("Reset board to run again.");

    while (true) { delay(1000); }
}
