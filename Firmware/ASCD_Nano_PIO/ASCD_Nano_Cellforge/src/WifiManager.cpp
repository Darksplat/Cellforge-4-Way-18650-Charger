#include <Arduino.h>
#include <WiFi.h>

#include "Secrets.h"
#include "StatusLED.h"
#include "DebugConfig.h"

constexpr uint8_t  MAX_WIFI_RETRIES = 10;
constexpr uint32_t WIFI_WINDOW_MS   = 3000;

bool connectToWiFi()
{
DBG_PRINTLN(F(""));
DBG_PRINTLN(F("========== WiFi BOOT ATTEMPT =========="));
DBG_PRINT(F("[WiFi] SSID: "));
DBG_PRINTLN(WIFI_SSID);


  StatusLED::setWifiState(WifiState::Scanning);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  for (uint8_t attempt = 1; attempt <= MAX_WIFI_RETRIES; attempt++)
  {
    DBG_PRINT(F("[WiFi] Attempt "));
    DBG_PRINT(attempt);
    DBG_PRINTLN(F("..."));

    uint32_t start = millis();

    while (millis() - start < WIFI_WINDOW_MS)
    {
      if (WiFi.status() == WL_CONNECTED)
      {
        DBG_PRINTLN(F("[WiFi] Connected"));
        DBG_PRINT(F("[WiFi] IP address: "));
        DBG_PRINTLN(WiFi.localIP());
        DBG_PRINTLN(F("[WiFi] WiFi initialisation complete"));


        StatusLED::setWifiState(WifiState::Connected);
        return true;
      }

      StatusLED::update();
      delay(5);
    }
  }

  DBG_PRINTLN(F("[WiFi] Connection FAILED after retries"));
  WiFi.disconnect(true);

  StatusLED::setWifiState(WifiState::Failed);
  return false;
}
