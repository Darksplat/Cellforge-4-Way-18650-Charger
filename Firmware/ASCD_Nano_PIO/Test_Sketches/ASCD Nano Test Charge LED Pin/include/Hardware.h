
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


#pragma once
#include <Arduino.h>

// ----------------------
// Pin assignments (Nano version)
// ----------------------

// Shift register 74HC595
constexpr uint8_t LATCH_PIN = 7;    // ST_CP
constexpr uint8_t CLOCK_PIN = 8;    // SH_CP
constexpr uint8_t DATA_PIN  = 6;    // DS

// Mux control pins (16-channel analog switch)
constexpr uint8_t MUX_S0 = 12;
constexpr uint8_t MUX_S1 = 11;
constexpr uint8_t MUX_S2 = 10;
constexpr uint8_t MUX_S3 = 9;

// Mux SIG input on A0
constexpr uint8_t MUX_SIG = A0;     // A0 == 14 on Nano

// Fan PWM (PCB v1.11+)
constexpr uint8_t FAN_PIN = 5;

// OneWire bus for DS18B20 sensors (PCB v1.11+)
constexpr uint8_t ONE_WIRE_BUS_PIN = 4;

// Number of modules / slots
constexpr uint8_t MODULE_COUNT = 4;

// ADC reference voltage (Nano 5V rail, measure & update this!)
constexpr float ADC_REFERENCE_VOLTAGE = 5.05f;


// ----------------------
// Module routing mapping
// (from original Brett Watt sketches)
// ----------------------

struct ModulePins {
  bool   batteryVoltageSel[4];
  bool   batteryVoltageDropSel[4];
  bool   chargeLedSel[4];
  uint8_t chargeMosfetShiftIndex;
  uint8_t dischargeMosfetShiftIndex;
};

// Same layout as in the original test sketches
static const ModulePins MODULES[MODULE_COUNT] = {
  // batteryVoltageSel,      batteryVoltageDropSel,      chargeLedSel,        charge, discharge
  { {1, 1, 0, 1},           {1, 1, 1, 1},              {0, 1, 0, 1},        0,      1 },
  { {1, 0, 0, 1},           {0, 1, 1, 1},              {0, 0, 0, 1},        2,      3 },
  { {1, 1, 1, 0},           {1, 0, 1, 1},              {0, 1, 1, 0},        4,      5 },
  { {1, 0, 1, 0},           {0, 0, 1, 1},              {0, 0, 1, 0},        6,      7 }
};


// ----------------------
// Initialisation helpers
// ----------------------

inline void initShiftRegister() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  digitalWrite(LATCH_PIN, LOW);
}

inline void initMux() {
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
}

inline void initFan() {
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
}


// ----------------------
// Shift register helper
// (replacement for digitalSwitch)
// ----------------------

// Keeps track of current 8 output states and updates only the bit you change.
inline void shiftRegisterSetOutput(uint8_t index, bool value) {
  static bool outputs[8] = { false, false, false, false, false, false, false, false };

  if (index >= 8) return;   // safety guard

  outputs[index] = value;

  uint8_t byteValue = 0;
  uint8_t baseTwo   = 1;

  for (uint8_t i = 0; i < 8; ++i) {
    if (outputs[i]) {
      byteValue += baseTwo;
    }
    baseTwo <<= 1;   // multiply by 2
  }

  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, byteValue);
  digitalWrite(LATCH_PIN, HIGH);
}


// ----------------------
// Mux helper
// ----------------------

// Select a mux channel based on the boolean selector array (like chargeLedSel[])
inline void muxSelect(const bool selector[4]) {
  const uint8_t controlPins[4] = { MUX_S0, MUX_S1, MUX_S2, MUX_S3 };
  for (uint8_t i = 0; i < 4; ++i) {
    digitalWrite(controlPins[i], selector[i] ? HIGH : LOW);
  }
}

// Read the mux SIG line and return the voltage, averaged over N samples.
inline float readMuxVoltage(const bool selector[4],
                            float referenceVoltage = ADC_REFERENCE_VOLTAGE,
                            uint8_t samples = 10) {
  muxSelect(selector);

  float sum = 0.0f;
  for (uint8_t i = 0; i < samples; ++i) {
    sum += analogRead(MUX_SIG);
  }

  float averageAdc = sum / samples;
  return averageAdc * referenceVoltage / 1023.0f;
}


// ----------------------
// Fan helper
// ----------------------

inline void setFanSpeed(uint8_t duty) {
  // 0–255, works on Nano PWM pin 5
  analogWrite(FAN_PIN, duty);
}
