/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"

// =====================================================
// Simple buzzer control
// =====================================================

void buzzer()
{
  if (soundBuzzer)
  {
    digitalWrite(PIN_BUZZ, HIGH);
    soundBuzzer = false;
  }
  else
  {
    digitalWrite(PIN_BUZZ, LOW);
  }
}
