#pragma once

#include <Arduino.h>
#include "WifiState.h"

class StatusLED
{
public:
  static void begin(uint8_t ledPin = LED_BUILTIN);
  static void setWifiState(WifiState state);
  static void update();

private:
  static uint8_t _pin;
  static WifiState _state;

  static uint32_t _lastChange;
  static bool _ledOn;

  // NEW: flash-based pattern state
  static uint8_t _flashCount;
  static bool _inPause;
};
