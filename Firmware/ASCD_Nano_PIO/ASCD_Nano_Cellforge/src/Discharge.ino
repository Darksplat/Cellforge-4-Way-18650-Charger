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
// Discharge cycle handler
// =====================================================

bool dischargeCycle(byte j)
{
  float batteryShuntVoltage = 0.00;

  module[j].intMilliSecondsCount +=
    (millis() - module[j].longMilliSecondsPreviousCount);
  module[j].longMilliSecondsPreviousCount = millis();

  // Take reading every interval or on first run
  if (module[j].intMilliSecondsCount >= settings.dischargeReadInterval ||
      module[j].dischargeAmps == 0)
  {
    // Battery voltage (board-side +BATx)
    int16_t rawBat = readADS1115(ADS_BAT_ADDR, j);
    module[j].dischargeVoltage = adsToVoltage(rawBat);

    // Load / shunt voltage (RVx)
    int16_t rawLoad = readADS1115(ADS_LOAD_ADDR, j);
    batteryShuntVoltage = adsToVoltage(rawLoad);

    if (module[j].dischargeVoltage >= settings.defaultBatteryCutOffVoltage)
    {
      digitalSwitch(module[j].dischargeMosfetPin, 1); // Enable discharge

      module[j].dischargeAmps =
        (module[j].dischargeVoltage - batteryShuntVoltage) /
        settings.shuntResistor[j];

      module[j].longMilliSecondsPassed =
        millis() - module[j].longMilliSecondsPrevious;

      module[j].dischargeMilliamps +=
        (module[j].dischargeAmps * 1000.0) *
        (module[j].longMilliSecondsPassed / 3600000.0);

      module[j].longMilliSecondsPrevious = millis();
    }

    module[j].intMilliSecondsCount = 0;

    // Below cutoff voltage → stop discharge
    if (module[j].dischargeVoltage < settings.defaultBatteryCutOffVoltage)
    {
      digitalSwitch(module[j].dischargeMosfetPin, 0);
      return true;
    }
  }

  return false;
}
