
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
#include "Hardware.h"   // shared pin + shift-register + mux helpers

// This sketch is a guided version of ASCD_Nano_Test_Charge_LED_Pin.
// It measures the TP5100 CHARGE LED pin voltage with:
//   - Batteries removed   -> "OFF" voltage
//   - Batteries inserted  -> "ON" voltage
// and prints a recommended mid threshold.

constexpr float BATTERY_VOLTAGE_LEAK = 0.50f;  // same as original

// Accumulators
float pinChargeVoltageOff = 0.0f;
float pinChargeVoltageOn  = 0.0f;


// ------------------ readVcc helper (from original) ------------------
// Measure the actual Vcc using the internal 1.1V reference.
// This helps compensate for USB/5V rail variations.
long readVcc()
{
    long result;
    // Read 1.1V reference against AVcc
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif

    delay(2);            // Wait for Vref to settle
    ADCSRA |= _BV(ADSC); // Start conversion
    while (bit_is_set(ADCSRA, ADSC)) {
        // wait
    }

    result  = ADCL;
    result |= ADCH << 8;
    result  = 1126400L / result; // 1126400 = 1.1 * 1024 * 1000 (mV)
    return result;               // in mV
}


// ------------------ Mux read helper using readVcc ------------------

float readMuxVoltageWithVcc(const bool selector[4], uint8_t samples = 10)
{
    // Use the same mux selection mapping as Hardware.h
    const uint8_t controlPins[4] = { MUX_S0, MUX_S1, MUX_S2, MUX_S3 };
    for (uint8_t i = 0; i < 4; i++) {
        digitalWrite(controlPins[i], selector[i] ? HIGH : LOW);
    }

    float sum = 0.0f;
    for (uint8_t i = 0; i < samples; i++) {
        // Scale ADC reading to volts using measured Vcc
        float vcc = readVcc() / 1000.0f; // in V
        sum += (analogRead(MUX_SIG) * (vcc / 1023.0f));
    }

    return sum / samples;
}


// ------------------ Utility: wait for ENTER in Serial ------------------

void waitForUser(const char* msg)
{
    Serial.println();
    Serial.println(msg);
    Serial.println("Press ENTER in Serial Monitor to continue...");

    while (!Serial.available()) {
        delay(10);
    }
    while (Serial.available()) {
        Serial.read(); // flush
    }
}


// ------------------ Setup ------------------

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("ASCD Nano - Charge LED Pin Guided Test");
    Serial.println("======================================");
    Serial.println("This test will measure the average voltage on the TP5100 CHARGE LED pin.");
    Serial.println();
    Serial.println("PREPARATION");
    Serial.println("-----------");
    Serial.println("1. REMOVE ALL BATTERIES from every slot.");
    Serial.println("2. Connect a stable 12 V supply to the DC jack.");
    Serial.println("3. Ensure the board is powered and TP5100 modules are present.");
    Serial.println();

    initShiftRegister();
    initMux();

    // Initially, set charge MOSFETs ON, discharge MOSFETs OFF like the original
    for (uint8_t i = 0; i < MODULE_COUNT; i++) {
        shiftRegisterSetOutput(MODULES[i].chargeMosfetShiftIndex, true);
        shiftRegisterSetOutput(MODULES[i].dischargeMosfetShiftIndex, false);
    }

    waitForUser("Make sure ALL batteries are removed and 12V is connected.");
}


// ------------------ Loop: one full measurement sequence ------------------

void loop()
{
    // 1) Measure Charge LED voltage with NO batteries (OFF condition)
    Serial.println();
    Serial.println("STEP 1: Measuring CHARGE LED pin voltage with NO batteries inserted...");
    Serial.println("Leave all slots empty during this step.");
    delay(1000);

    pinChargeVoltageOff = 0.0f;

    // Take 10 sets of readings over all modules
    for (uint8_t sampleSet = 0; sampleSet < 10; sampleSet++) {
        for (uint8_t m = 0; m < MODULE_COUNT; m++) {
            float v = readMuxVoltageWithVcc(MODULES[m].chargeLedSel, 5);
            pinChargeVoltageOff += v;

            Serial.print("Module ");
            Serial.print(m + 1);
            Serial.print(" CHARGE LED = ");
            Serial.print(v, 3);
            Serial.println(" V (batt OUT)");
            delay(250);
        }
        delay(500);
    }

    // Average across 10*MODULE_COUNT samples
    pinChargeVoltageOff /= (10.0f * MODULE_COUNT);

    Serial.println();
    Serial.print("Average CHARGE LED pin voltage with NO batteries: ");
    Serial.print(pinChargeVoltageOff, 3);
    Serial.println(" V");
    Serial.println();


    // 2) Ask user to insert batteries and verify all slots have a cell
    Serial.println("STEP 2: Insert batteries that need charging into ALL slots.");
    Serial.println("The test will wait until it detects a battery in each module.");
    Serial.println();

    // Pulse each module's charge MOSFET to pre-discharge the ADC a bit (as in original)
    for (uint8_t i = 0; i < MODULE_COUNT; i++) {
        // small pulse sequence to stabilise readings
        shiftRegisterSetOutput(MODULES[i].chargeMosfetShiftIndex, true);
        delay(500);
        shiftRegisterSetOutput(MODULES[i].chargeMosfetShiftIndex, false);
        delay(500);

        // pre read batteryVoltageSel to pull down any stray voltage
        (void)readMuxVoltageWithVcc(MODULES[i].batteryVoltageSel, 5);
        shiftRegisterSetOutput(MODULES[i].chargeMosfetShiftIndex, true);
        delay(500);
    }

    delay(5000);

    bool batteriesInserted = false;
    uint8_t batteriesInsertedCount = 0;

    while (!batteriesInserted) {
        batteriesInsertedCount = 0;

        for (uint8_t i = 0; i < MODULE_COUNT; i++) {
            float vBatt = readMuxVoltageWithVcc(MODULES[i].batteryVoltageSel, 5);
            if (vBatt >= BATTERY_VOLTAGE_LEAK) {
                batteriesInsertedCount++;
            }

            Serial.print("Module ");
            Serial.print(i + 1);
            Serial.print(" battery sense = ");
            Serial.print(vBatt, 3);
            Serial.println(" V");
            delay(250);
        }

        if (batteriesInsertedCount >= MODULE_COUNT) {
            batteriesInserted = true;
        } else {
            Serial.println("Not all slots show a battery yet. Waiting 1s and checking again...");
        }

        delay(1000);
    }

    Serial.println();
    Serial.println("All modules show a battery present.");
    Serial.println("Do NOT remove the batteries during the next measurement.");
    Serial.println();


    // 3) Measure Charge LED voltage with batteries inserted (ON condition)
    Serial.println("STEP 3: Measuring CHARGE LED pin voltage with batteries inserted...");
    delay(1000);

    pinChargeVoltageOn = 0.0f;

    for (uint8_t sampleSet = 0; sampleSet < 10; sampleSet++) {
        for (uint8_t m = 0; m < MODULE_COUNT; m++) {
            float v = readMuxVoltageWithVcc(MODULES[m].chargeLedSel, 5);
            pinChargeVoltageOn += v;

            Serial.print("Module ");
            Serial.print(m + 1);
            Serial.print(" CHARGE LED = ");
            Serial.print(v, 3);
            Serial.println(" V (batt IN)");
            delay(250);
        }
        delay(500);
    }

    pinChargeVoltageOn /= (10.0f * MODULE_COUNT);

    Serial.println();
    Serial.print("Average CHARGE LED pin voltage with batteries INSERTED: ");
    Serial.print(pinChargeVoltageOn, 3);
    Serial.println(" V");
    Serial.println();


    // 4) Compute recommended mid threshold
    float chargeLedMid = (pinChargeVoltageOn + pinChargeVoltageOff) * 0.5f;

    Serial.println("RESULTS");
    Serial.println("-------");
    Serial.print("Average OFF voltage : ");
    Serial.print(pinChargeVoltageOff, 3);
    Serial.println(" V");
    Serial.print("Average ON voltage  : ");
    Serial.print(pinChargeVoltageOn, 3);
    Serial.println(" V");
    Serial.print("Recommended MID     : ");
    Serial.print(chargeLedMid, 3);
    Serial.println(" V");
    Serial.println();

    Serial.println("You can use this value as a global Charge LED pin threshold, or");
    Serial.println("compare it with the per-module values from the Mid-Voltage test.");
    Serial.println();
    Serial.println("Reset the board to run the test again.");
    Serial.println("Looping idle now.");

    while (true) {
        delay(1000);
    }
}
