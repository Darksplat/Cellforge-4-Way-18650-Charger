#include "StatusLED.h"

// ----------------------
// Flash timing (human obvious)
// ----------------------
constexpr uint16_t FLASH_ON_MS        = 150;   // short, sharp flash
constexpr uint16_t FLASH_OFF_MS       = 600;   // clearly OFF between flashes
constexpr uint16_t SEQUENCE_PAUSE_MS  = 1500;  // pause after full pattern

uint8_t StatusLED::_pin = LED_BUILTIN;
WifiState StatusLED::_state = WifiState::Scanning;

uint32_t StatusLED::_lastChange = 0;
bool StatusLED::_ledOn = false;
uint8_t StatusLED::_flashCount = 0;
bool StatusLED::_inPause = false;

void StatusLED::begin(uint8_t ledPin)
{
  _pin = ledPin;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);
}

void StatusLED::setWifiState(WifiState state)
{
  _state = state;
  _flashCount = 0;
  _ledOn = false;
  _inPause = false;
  digitalWrite(_pin, LOW);
  _lastChange = millis();
}

void StatusLED::update()
{
  uint32_t now = millis();

  // Connected = solid ON
  if (_state == WifiState::Connected)
  {
    digitalWrite(_pin, HIGH);
    return;
  }

  // How many flashes this state needs
  uint8_t flashesRequired =
    (_state == WifiState::Scanning) ? 2 : 3;

  // Handle pause between sequences
  if (_inPause)
  {
    if (now - _lastChange >= SEQUENCE_PAUSE_MS)
    {
      _inPause = false;
      _flashCount = 0;
      _lastChange = now;
    }
    return;
  }

  // LED currently ON → turn OFF after flash time
  if (_ledOn)
  {
    if (now - _lastChange >= FLASH_ON_MS)
    {
      digitalWrite(_pin, LOW);
      _ledOn = false;
      _lastChange = now;
    }
    return;
  }

  // LED currently OFF → decide next action
  if (!_ledOn)
  {
    if (_flashCount < flashesRequired)
    {
      if (now - _lastChange >= FLASH_OFF_MS)
      {
        digitalWrite(_pin, HIGH);
        _ledOn = true;
        _flashCount++;
        _lastChange = now;
      }
    }
    else
    {
      // Finished sequence → enter pause
      _inPause = true;
      _lastChange = now;
    }
  }
}
