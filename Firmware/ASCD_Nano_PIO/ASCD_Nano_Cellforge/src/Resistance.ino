/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"


// =====================================================
// External helpers from IOUtils.ino
// =====================================================
extern int16_t readADS1115(uint8_t i2cAddr, uint8_t channel);
extern float   adsToVoltage(int16_t raw);


// =====================================================
// Internal resistance (milliOhms) measurement
// =====================================================

byte milliOhms(byte j)
{
  float resistanceAmps      = 0.0;
  float voltageDrop         = 0.0;
  float batteryOpenVoltage  = 0.0;
  float batteryLoadVoltage  = 0.0;

  // Ensure discharge is OFF (open-circuit)
  digitalSwitch(module[j].dischargeMosfetPin, 0);
  delay(20); // allow voltage to settle

  int16_t rawOpen = readADS1115(ADS_BAT_ADDR, j);
  batteryOpenVoltage = adsToVoltage(rawOpen);

  // Apply load
  digitalSwitch(module[j].dischargeMosfetPin, 1);
  delay(20); // allow load to settle

  int16_t rawLoad = readADS1115(ADS_BAT_ADDR, j);
  batteryLoadVoltage = adsToVoltage(rawLoad);

  // Remove load
  digitalSwitch(module[j].dischargeMosfetPin, 0);

  // Calculate current through load
  resistanceAmps =
    (batteryOpenVoltage - batteryLoadVoltage) /
    settings.shuntResistor[j];

  // Guard against divide-by-zero
  if (resistanceAmps <= 0.001)
  {
    module[j].milliOhmsValue = 9999;
    return 1;
  }

  voltageDrop = batteryOpenVoltage - batteryLoadVoltage;

  module[j].milliOhmsValue =
    ((voltageDrop / resistanceAmps) * 1000.0) +
    settings.offsetMilliOhms;

  if (module[j].milliOhmsValue > 9999)
  {
    module[j].milliOhmsValue = 9999;
  }

  return 1;
}
