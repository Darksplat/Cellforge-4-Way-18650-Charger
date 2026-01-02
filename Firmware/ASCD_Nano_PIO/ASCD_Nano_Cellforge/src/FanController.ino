/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"

// =====================================================
// Fan controller (ESP32-safe PWM)
// =====================================================

void fanController()
{
  static bool fanOn = false;
  const byte fanTempMin = 28; // °C
  const byte fanTempMax = 38; // °C
  bool dischargeFanOn = false;

  // Force fan full speed if any module is discharging
  for (byte j = 0; j < settings.moduleCount; j++)
  {
    if (module[j].cycleState == 5) // Discharge state
    {
      dischargeFanOn = true;
      break;
    }
  }

  if (dischargeFanOn)
  {
    ledcWrite(FAN_PWM_CHANNEL, 255);
    fanOn = true;
    return;
  }

  if (ambientTemperature < fanTempMin)
  {
    ledcWrite(FAN_PWM_CHANNEL, 0);
    fanOn = false;
  }
  else if (ambientTemperature < fanTempMax)
  {
    int fanSpeed = map(ambientTemperature,
                       fanTempMin,
                       fanTempMax,
                       settings.pwmFanMinStart,
                       252);

    if (!fanOn)
    {
      // Kick-start fan
      ledcWrite(FAN_PWM_CHANNEL, 255);
      fanOn = true;
    }
    else
    {
      ledcWrite(FAN_PWM_CHANNEL, fanSpeed);
    }
  }
  else
  {
    ledcWrite(FAN_PWM_CHANNEL, 255);
    fanOn = true;
  }
}
