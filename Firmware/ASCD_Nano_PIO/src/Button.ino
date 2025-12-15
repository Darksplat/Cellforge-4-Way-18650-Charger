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

// Button.ino
// Handles the front-panel button.

void button()
{
  boolean buttonState      = 0;
  static boolean lastState = 0;

  buttonState = digitalRead(BTN);

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
