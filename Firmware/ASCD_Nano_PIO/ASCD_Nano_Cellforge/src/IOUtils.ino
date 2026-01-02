/*
  ASDC Nano 4x Arduino Charger / Discharger
  Created by Brett Watt on 19/03/2019
  Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include <Wire.h>
#include "Pins.h"

// =====================================================
// ADS1115 minimal helper (no external library)
// =====================================================

int16_t readADS1115(uint8_t i2cAddr, uint8_t channel)
{
  uint16_t config =
    0x8000 |                    // OS
    ((0x4 + channel) << 12) |   // MUX AINx
    0x0200 |                    // PGA ±2.048V
    0x0100 |                    // single-shot
    0x0080 |                    // 128 SPS
    0x0003;                     // disable comparator

  Wire.beginTransmission(i2cAddr);
  Wire.write(0x01);
  Wire.write(config >> 8);
  Wire.write(config & 0xFF);
  Wire.endTransmission();

  delay(9);

  Wire.beginTransmission(i2cAddr);
  Wire.write(0x00);
  Wire.endTransmission();

  Wire.requestFrom(i2cAddr, (uint8_t)2);
  return (int16_t)((Wire.read() << 8) | Wire.read());
}

float adsToVoltage(int16_t raw)
{
  float v = raw * 0.0000625f; // 62.5uV/LSB @ ±2.048V
  return v * 2.0f;            // 100k/100k divider compensation
}

// =====================================================
// Battery presence check (ADS1115 channel == module index)
// =====================================================

bool batteryCheck(byte j)
{
  int16_t raw = readADS1115(ADS_BAT_ADDR, j);
  module[j].batteryVoltage = adsToVoltage(raw);
  return (module[j].batteryVoltage > settings.batteryVolatgeLeak);
}

// =====================================================
// Shift register control (unchanged)
// =====================================================

void digitalSwitch(byte j, bool value)
{
  byte baseTwo = 1;
  byte eightBitDecimal = 0;
  static bool digitalPinsState[8];

  digitalPinsState[j] = value;

  for (byte i = 0; i < 8; i++)
  {
    if (digitalPinsState[i]) eightBitDecimal += baseTwo;
    baseTwo <<= 1;
  }

  digitalWrite(PIN_SR_LATCH, LOW);
  shiftOut(PIN_SR_DATA, PIN_SR_CLOCK, MSBFIRST, eightBitDecimal);
  digitalWrite(PIN_SR_LATCH, HIGH);
}

// =====================================================
// Legacy readMux compatibility wrapper (routes to ADS)
// =====================================================

float readMux(const bool inputArray[])
{
  byte index =
    (inputArray[0] ? 1 : 0) |
    (inputArray[1] ? 2 : 0) |
    (inputArray[2] ? 4 : 0) |
    (inputArray[3] ? 8 : 0);

  int16_t raw = readADS1115(ADS_BAT_ADDR, index);
  return adsToVoltage(raw);
}
