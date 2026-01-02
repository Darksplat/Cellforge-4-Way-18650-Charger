/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"


// =====================================================
// Temperature processing for modules and ambient
// =====================================================

byte processTemperature(byte j)
{
  module[j].batteryCurrentTemp = getTemperature(j);

  // Track highest temperature (ignore invalid 99)
  if (module[j].batteryCurrentTemp > module[j].batteryHighestTemp &&
      module[j].batteryCurrentTemp != 99)
  {
    module[j].batteryHighestTemp = module[j].batteryCurrentTemp;
  }

  if ((module[j].batteryCurrentTemp - ambientTemperature) > settings.tempThreshold &&
      module[j].batteryCurrentTemp != 99)
  {
    if ((module[j].batteryCurrentTemp - ambientTemperature) > settings.tempMaxThreshold)
    {
      // Above maximum threshold = fault
      return 2;
    }
    else
    {
      // Above warning threshold
      return 1;
    }
  }

  // Within safe range
  return 0;
}


// =====================================================
// Per-module temperature read
// =====================================================

byte getTemperature(byte j)
{
  if (module[j].tempCount > 16 ||
      module[j].batteryCurrentTemp == 0 ||
      module[j].batteryCurrentTemp == 99)
  {
    module[j].tempCount = 0;

    sensors.requestTemperaturesByAddress(tempSensorSerial[j]);
    float tempC = sensors.getTempC(tempSensorSerial[j]);

    if (tempC > 99.0 || tempC < 0.0)
    {
      // Sensor error
      tempC = (module[j].batteryCurrentTemp != 99)
               ? module[j].batteryCurrentTemp
               : 99.0;
    }

    return (byte)tempC;
  }
  else
  {
    module[j].tempCount++;
    return module[j].batteryCurrentTemp;
  }
}


// =====================================================
// Ambient temperature read (sensor index 4)
// =====================================================

void getAmbientTemperature()
{
  static byte ambientTempCount;

  if (ambientTempCount > 16 ||
      ambientTemperature == 0 ||
      ambientTemperature == 99)
  {
    ambientTempCount = 0;

    sensors.requestTemperaturesByAddress(tempSensorSerial[4]);
    float tempC = sensors.getTempC(tempSensorSerial[4]);

    if (tempC > 99.0 || tempC < 0.0)
    {
      tempC = (ambientTemperature != 99)
               ? ambientTemperature
               : 99.0;
    }

    ambientTemperature = (byte)tempC;
  }
  else
  {
    ambientTempCount++;
  }
}
