
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
#include "Hardware.h"

// I2C 16x2 LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Per-module measured values
float ledOffVoltage[MODULE_COUNT];   // LED "off" voltage
float ledOnVoltage[MODULE_COUNT];    // LED "on" voltage
float ledMidVoltage[MODULE_COUNT];   // Recommended mid threshold


// ------------- Utility: wait for user via Serial + show on LCD -------------

void waitForUser(const char* line1, const char* line2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);

  Serial.println();
  Serial.println(line1);
  Serial.println(line2);
  Serial.println("Press ENTER in Serial Monitor to continue...");

  // Wait for any character
  while (!Serial.available()) {
    delay(10);
  }
  // Flush the input
  while (Serial.available()) {
    Serial.read();
  }
}


// ------------- Measurement helpers -------------

// Measure the charge LED pin for a given module, averaged over N samples
float measureChargeLedVoltage(uint8_t moduleIndex, uint8_t samples = 10)
{
  const bool* sel = MODULES[moduleIndex].chargeLedSel;
  return readMuxVoltage(sel, ADC_REFERENCE_VOLTAGE, samples);
}


// ------------- Setup & main procedure -------------

void setup()
{
  Serial.begin(115200);

  // LCD init
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Charge LED Mid");
  lcd.setCursor(0, 1);
  lcd.print("Voltage Test");
  delay(2000);

  // Hardware init
  initShiftRegister();
  initMux();

  Serial.println("ASCD Nano - Charge LED Mid Voltage Guided Test");
  Serial.println("------------------------------------------------");
  Serial.println("Make sure:");
  Serial.println(" - All batteries are REMOVED");
  Serial.println(" - 12 V power is connected to the DC jack");
  Serial.println(" - TP5100 modules are powered.");
  Serial.println();

  waitForUser("Remove batteries", "Connect 12V, press OK");

  // MAIN MEASUREMENT LOOP
  for (uint8_t m = 0; m < MODULE_COUNT; ++m)
  {
    // Small status on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Measuring Mod ");
    lcd.print(m + 1);

    Serial.println();
    Serial.print("=== Module ");
    Serial.print(m + 1);
    Serial.println(" ===");

    // Ensure MOSFETs are OFF first
    shiftRegisterSetOutput(MODULES[m].chargeMosfetShiftIndex,    false);
    shiftRegisterSetOutput(MODULES[m].dischargeMosfetShiftIndex, false);
    delay(500);

    // Measure LED "off" voltage
    float vOff = measureChargeLedVoltage(m, 10);
    ledOffVoltage[m] = vOff;
    Serial.print("Charge LED OFF voltage: ");
    Serial.print(vOff, 3);
    Serial.println(" V");

    lcd.setCursor(0, 1);
    lcd.print("OFF:");
    lcd.print(vOff, 2);
    lcd.print("V ");

    delay(1500);

    // Turn ON both MOSFETs (same idea as original test)
    shiftRegisterSetOutput(MODULES[m].chargeMosfetShiftIndex,    true);
    shiftRegisterSetOutput(MODULES[m].dischargeMosfetShiftIndex, true);
    delay(500);

    // Measure LED "on" voltage
    float vOn = measureChargeLedVoltage(m, 10);
    ledOnVoltage[m] = vOn;
    Serial.print("Charge LED ON voltage : ");
    Serial.print(vOn, 3);
    Serial.println(" V");

    lcd.setCursor(0, 1);
    lcd.print("ON :");
    lcd.print(vOn, 2);
    lcd.print("V ");

    delay(1500);

    // Turn MOSFETs OFF again
    shiftRegisterSetOutput(MODULES[m].chargeMosfetShiftIndex,    false);
    shiftRegisterSetOutput(MODULES[m].dischargeMosfetShiftIndex, false);

    // Compute mid threshold (simple midpoint)
    float vMid = (vOff + vOn) * 0.5f;
    ledMidVoltage[m] = vMid;

    Serial.print("Recommended MID value: ");
    Serial.print(vMid, 3);
    Serial.println(" V");
    Serial.println();
  }

  // After measuring all modules, print the summary line
  Serial.println("------------------------------------------------");
  Serial.println("Copy this line into your main ASCD_Nano sketch:");
  Serial.println();

  Serial.print("const float chargeLedPinMidVoltage[");
  Serial.print(MODULE_COUNT);
  Serial.print("] = {");
  for (uint8_t m = 0; m < MODULE_COUNT; ++m)
  {
    Serial.print(ledMidVoltage[m], 3);
    if (m < MODULE_COUNT - 1) {
      Serial.print(", ");
    }
  }
  Serial.println("};");
  Serial.println();
  Serial.println("You may also want to keep a note of OFF/ON values for diagnostics:");
  for (uint8_t m = 0; m < MODULE_COUNT; ++m)
  {
    Serial.print("Module ");
    Serial.print(m + 1);
    Serial.print(" OFF=");
    Serial.print(ledOffVoltage[m], 3);
    Serial.print(" V, ON=");
    Serial.print(ledOnVoltage[m], 3);
    Serial.print(" V, MID=");
    Serial.print(ledMidVoltage[m], 3);
    Serial.println(" V");
  }

  // Show a summary on the LCD and then let loop() just cycle them.
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mid Voltages Set");
  lcd.setCursor(0, 1);
  lcd.print("See Serial Log");
  delay(3000);
}

void loop()
{
  // Gently cycle through the per-module mid values on the LCD
  for (uint8_t m = 0; m < MODULE_COUNT; ++m)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Module ");
    lcd.print(m + 1);
    lcd.print(" MidVolt");

    lcd.setCursor(0, 1);
    lcd.print(ledMidVoltage[m], 3);
    lcd.print(" V          ");

    delay(4000);
  }
}
