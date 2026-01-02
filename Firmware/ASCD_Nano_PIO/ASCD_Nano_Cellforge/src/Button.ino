/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"

// =====================================================
// Button handler
// =====================================================

void button()
{
  bool buttonState = false;
  static bool lastState = false;

  buttonState = digitalRead(PIN_BTN);

  if (buttonState != lastState)
  {
    if (buttonState == LOW) // active on press
    {
      buttonPressed = true;
      soundBuzzer   = true;
    }
  }

  lastState = buttonState;
}
