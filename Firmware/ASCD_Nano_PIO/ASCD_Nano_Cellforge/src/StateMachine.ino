/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>
#include "Pins.h"


// =====================================================
// External helpers (defined in IOUtils.ino)
// NOTE: Ensure these are NOT declared "static" in IOUtils.ino
// =====================================================
extern int16_t readADS1115(uint8_t i2cAddr, uint8_t channel);
extern float   adsToVoltage(int16_t raw);


// Small local helper: read current battery voltage for slot j (BVj via ADS1115)
static inline float readBatteryVoltage(byte j)
{
  return adsToVoltage(readADS1115(ADS_BAT_ADDR, j));
}


// =====================================================
// Core state machine
// =====================================================

void cycleStateValues()
{
  strcpy(serialSendString, "");
  getAmbientTemperature();
  sprintf_P(serialSendString + strlen(serialSendString), PSTR("&AT=%d"), ambientTemperature);

  for (byte i = 0; i < settings.moduleCount; i++)
  {
    switch (module[i].cycleState)
    {
      case 0: // Check Battery Voltage
        if (batteryCheck(i))
          module[i].cycleCount++;

        if (module[i].cycleCount == 5)
        {
          initializeVariables(i);
          module[i].batteryCurrentTemp = getTemperature(i);
          module[i].batteryInitialTemp = module[i].batteryCurrentTemp;
          module[i].batteryHighestTemp = module[i].batteryCurrentTemp;

          clearSecondsTimer(i);

          // Battery voltage now comes from ADS1115
          module[i].batteryVoltage        = readBatteryVoltage(i);
          module[i].batteryInitialVoltage = module[i].batteryVoltage;

          module[i].cycleState = 1; // -> barcode
          module[i].cycleCount = 0;
        }

        sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=0"), i);
        break;

      case 1: // Battery Barcode
        module[i].batteryVoltage = readBatteryVoltage(i);

        if (module[i].batteryBarcode == true)
        {
          clearSecondsTimer(i);
          module[i].batteryInitialVoltage = module[i].batteryVoltage;
          module[i].cycleState = 2; // -> charge
        }

        // Battery removed?
        if (!batteryCheck(i))
          module[i].cycleCount++;

        if (module[i].cycleCount == 5)
        {
          module[i].cycleState = 0;
          module[i].cycleCount = 0;
        }

        sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=1"), i);
        break;

      case 2: // Charge Battery
        module[i].batteryVoltage = readBatteryVoltage(i);

        sprintf_P(serialSendString + strlen(serialSendString),
                  PSTR("&CS%d=2&TI%d=%d&IT%d=%d&IV%d=%d.%02d&CT%d=%d&CV%d=%d.%02d&HT%d=%d"),
                  i, i,
                  (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)),
                  i, module[i].batteryInitialTemp,
                  i, (int)module[i].batteryInitialVoltage, (int)(module[i].batteryInitialVoltage * 100) % 100,
                  i, module[i].batteryCurrentTemp,
                  i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100,
                  i, module[i].batteryHighestTemp);

        if (processTemperature(i) == 2)
        {
          digitalSwitch(module[i].chargeMosfetPin, 0);
          module[i].batteryFaultCode = 7;

          if (module[i].insertData == true)
          {
            clearSecondsTimer(i);
            module[i].insertData = false;
            module[i].cycleState = 7;
            module[i].cycleCount = 0;
          }
          sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
        }
        else
        {
          digitalSwitch(module[i].chargeMosfetPin, 1);

          module[i].cycleCount = module[i].cycleCount + chargeCycle(i);
          if (module[i].cycleCount >= 10)
          {
            digitalSwitch(module[i].chargeMosfetPin, 0);

            if (module[i].insertData == true)
            {
              module[i].insertData = false;
              module[i].cycleState = 3; // -> resistance
              module[i].cycleCount = 0;
            }
            sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
          }
        }

        if (module[i].hours == settings.chargingTimeout)
        {
          digitalSwitch(module[i].chargeMosfetPin, 0);
          module[i].batteryFaultCode = 9;

          if (module[i].insertData == true)
          {
            clearSecondsTimer(i);
            module[i].insertData = false;
            module[i].cycleState = 7;
            module[i].cycleCount = 0;
          }
          sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
        }
        break;

      case 3: // Check Battery Milli Ohms
        module[i].cycleCount = module[i].cycleCount + milliOhms(i);
        module[i].tempMilliOhmsValue = module[i].tempMilliOhmsValue + module[i].milliOhmsValue;

        if (module[i].cycleCount == 4)
        {
          module[i].milliOhmsValue = module[i].tempMilliOhmsValue / 4;

          if (module[i].milliOhmsValue > settings.highMilliOhms)
          {
            module[i].batteryFaultCode = 3;
            module[i].cycleState = 7;
            module[i].cycleCount = 0;
          }
          else
          {
            if (module[i].minutes <= 1)
            {
              module[i].cycleState = 5;
              module[i].cycleCount = 0;
            }
            else
            {
              module[i].cycleState = 4;
              module[i].cycleCount = 0;
            }
          }
          clearSecondsTimer(i);
        }

        sprintf_P(serialSendString + strlen(serialSendString),
                  PSTR("&CS%d=3&MO%d=%d&CV%d=%d.%02d"),
                  i, i, (int)module[i].milliOhmsValue,
                  i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100);
        break;

      case 4: // Rest Battery
        module[i].batteryVoltage = readBatteryVoltage(i);
        module[i].batteryCurrentTemp = getTemperature(i);

        if (module[i].minutes == settings.restTimeMinutes)
        {
          module[i].batteryInitialVoltage = module[i].batteryVoltage;
          clearSecondsTimer(i);
          module[i].cycleState = 5;
        }

        sprintf_P(serialSendString + strlen(serialSendString),
                  PSTR("&CS%d=4&TI%d=%d&CT%d=%d&CV%d=%d.%02d"),
                  i, i,
                  (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)),
                  i, module[i].batteryCurrentTemp,
                  i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100);
        break;

      case 5: // Discharge Battery
        sprintf_P(serialSendString + strlen(serialSendString),
                  PSTR("&CS%d=5&TI%d=%d&IT%d=%d&IV%d=%d.%02d&CT%d=%d&CV%d=%d.%02d&HT%d=%d&MA%d=%d&DA%d=%d.%02d&MO%d=%d"),
                  i, i,
                  (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)),
                  i, module[i].batteryInitialTemp,
                  i, (int)module[i].batteryInitialVoltage, (int)(module[i].batteryInitialVoltage * 100) % 100,
                  i, module[i].batteryCurrentTemp,
                  i, (int)module[i].dischargeVoltage, (int)(module[i].dischargeVoltage * 100) % 100,
                  i, module[i].batteryHighestTemp,
                  i, (int)module[i].dischargeMilliamps,
                  i, (int)module[i].dischargeAmps, (int)(module[i].dischargeAmps * 100) % 100,
                  i, (int)module[i].milliOhmsValue);

        if (processTemperature(i) == 2)
        {
          digitalSwitch(module[i].dischargeMosfetPin, 0);
          module[i].batteryFaultCode = 7;

          if (module[i].insertData == true)
          {
            clearSecondsTimer(i);
            module[i].insertData = false;
            module[i].cycleState = 7;
            module[i].cycleCount = 0;
          }
          sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
        }
        else
        {
          if (dischargeCycle(i))
            module[i].cycleCount++;

          if (module[i].cycleCount >= 10)
          {
            digitalSwitch(module[i].dischargeMosfetPin, 0);

            if (module[i].dischargeMilliamps < settings.lowMilliamps)
            {
              module[i].batteryFaultCode = 5;

              if (module[i].insertData == true)
              {
                clearSecondsTimer(i);
                module[i].insertData = false;
                module[i].cycleState = 7;
                module[i].cycleCount = 0;
              }
              sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
            }
            else
            {
              if (module[i].insertData == true)
              {
                module[i].batteryVoltage        = readBatteryVoltage(i);
                module[i].batteryInitialVoltage = module[i].batteryVoltage;

                clearSecondsTimer(i);
                module[i].insertData = false;
                module[i].cycleState = 6;
                module[i].cycleCount = 0;
              }
              sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
            }
          }
        }
        break;

      case 6: // Recharge Battery
        module[i].batteryVoltage = readBatteryVoltage(i);

        sprintf_P(serialSendString + strlen(serialSendString),
                  PSTR("&CS%d=6&TI%d=%d&IT%d=%d&IV%d=%d.%02d&CT%d=%d&CV%d=%d.%02d&HT%d=%d"),
                  i, i,
                  (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)),
                  i, module[i].batteryInitialTemp,
                  i, (int)module[i].batteryInitialVoltage, (int)(module[i].batteryInitialVoltage * 100) % 100,
                  i, module[i].batteryCurrentTemp,
                  i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100,
                  i, module[i].batteryHighestTemp);

        if (processTemperature(i) == 2)
        {
          digitalSwitch(module[i].chargeMosfetPin, 0);
          module[i].batteryFaultCode = 7;

          if (module[i].insertData == true)
          {
            clearSecondsTimer(i);
            module[i].insertData = false;
            module[i].cycleState = 7;
            module[i].cycleCount = 0;
          }
          sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
        }
        else
        {
          digitalSwitch(module[i].chargeMosfetPin, 1);

          if (settings.storageChargeVoltage > 0.00)
          {
            if (module[i].batteryVoltage > (settings.storageChargeVoltage + 0.35))
              module[i].cycleCount++;
          }
          else
          {
            module[i].cycleCount = module[i].cycleCount + chargeCycle(i);
          }

          if (module[i].cycleCount >= 10)
          {
            digitalSwitch(module[i].chargeMosfetPin, 0);

            if (module[i].insertData == true)
            {
              clearSecondsTimer(i);
              module[i].insertData = false;
              module[i].cycleState = 7;
              module[i].cycleCount = 0;
            }
            sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
          }
        }

        if (module[i].hours == settings.chargingTimeout)
        {
          digitalSwitch(module[i].chargeMosfetPin, 0);
          module[i].batteryFaultCode = 9;

          if (module[i].insertData == true)
          {
            clearSecondsTimer(i);
            module[i].insertData = false;
            module[i].cycleState = 7;
            module[i].cycleCount = 0;
          }
          sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
        }
        break;

      case 7: // Completed
        if (!batteryCheck(i))
          module[i].cycleCount++;

        if (module[i].cycleCount == 2)
        {
          module[i].cycleState = 0;
          module[i].cycleCount = 0;
        }

        sprintf_P(serialSendString + strlen(serialSendString),
                  PSTR("&CS%d=7&CV%d=%d.%02d&FC%d=%d"),
                  i, i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100,
                  i, module[i].batteryFaultCode);
        break;
    }

    secondsTimer(i);
  }

  cycleStateLCD();
  fanController();
}
