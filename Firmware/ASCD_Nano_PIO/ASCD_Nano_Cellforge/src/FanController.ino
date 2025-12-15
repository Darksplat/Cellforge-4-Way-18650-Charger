
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

// FanController.ino
// Controls the cooling fan based on temperature and discharge state.

void fanController()
{
  static boolean fanOn = 0;
  const byte fanTempMin = 28; // Start fan at this temperature (°C)
  const byte fanTempMax = 38; // Max temperature (°C) for full speed
  boolean dischargeFanOn = false;

  // If any module is discharging, force fan full speed
  for (byte j = 0; j < settings.moduleCount; j++)
  {
    if (module[j].cycleState == 5) // Discharge state
    {
      dischargeFanOn = true;
    }
  }

  if (dischargeFanOn == true)
  {
    digitalWrite(FAN, HIGH);
    return;
  }

  if (ambientTemperature < fanTempMin)
  {
    digitalWrite(FAN, LOW);
    fanOn = 0;
  }
  else if (ambientTemperature < fanTempMax)
  {
    int fanSpeed = map(ambientTemperature,
                       fanTempMin,
                       fanTempMax,
                       settings.pwmFanMinStart,
                       252);

    if (fanOn == 0)
    {
      fanSpeed = 255;
      fanOn = 1;
    }
    else
    {
      analogWrite(FAN, fanSpeed);
    }
  }
  else
  {
    digitalWrite(FAN, HIGH);
    fanOn = 1;
  }
}
