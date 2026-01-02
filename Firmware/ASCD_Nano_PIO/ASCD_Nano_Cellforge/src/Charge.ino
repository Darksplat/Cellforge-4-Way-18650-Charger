/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"


// =====================================================
// Charge cycle handler
// Determines whether charging is complete for a module
// =====================================================

bool chargeCycle(byte j)
{
  bool chargeDone = false;

  switch (j)
  {
    case 0:
      chargeDone = digitalRead(PIN_CD1);
      break;

    case 1:
      chargeDone = digitalRead(PIN_CD2);
      break;

    case 2:
      chargeDone = digitalRead(PIN_CD3);
      break;

    case 3:
      chargeDone = digitalRead(PIN_CD4);
      break;

    default:
      return false;
  }

  // TP5100: HIGH = charge complete, LOW = charging
  return chargeDone;
}
