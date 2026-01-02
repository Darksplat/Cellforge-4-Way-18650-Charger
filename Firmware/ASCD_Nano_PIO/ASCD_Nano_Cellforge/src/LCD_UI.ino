/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>

// =====================================================
// LCD user interface
// =====================================================

void cycleStateLCD()
{
  static byte screenOverride   = 0;
  static byte cycleStateCount  = 0;
  static byte cycleStateActive = 0;

  if (screenOverride > 0 && !buttonPressed)
  {
    // Lock mode active
    screenOverride--;
    cycleStateLCDOutput(cycleStateActive);
  }
  else if (screenOverride == 0 && buttonPressed)
  {
    // Enter lock mode
    lcd.setCursor(0, 0);
    lcd.print(F("LOCK MODE 1 MIN "));
    lcd.setCursor(0, 1);
    lcd.print(F("                "));
    screenOverride = 60;
    buttonPressed  = false;
  }
  else
  {
    // Rotate through modules
    if (cycleStateCount == settings.screenTime || buttonPressed)
    {
      cycleStateActive =
        (cycleStateActive == (settings.moduleCount - 1)) ? 0 : cycleStateActive + 1;

      cycleStateCount = 0;
      buttonPressed   = false;
    }
    else
    {
      cycleStateCount++;
    }

    cycleStateLCDOutput(cycleStateActive);
  }
}


void cycleStateLCDOutput(byte j)
{
  char lcdLine0[20];
  char lcdLine1[20];

  switch (module[j].cycleState)
  {
    case 0: // Battery check
      sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-BATTERY CHECK"));
      sprintf_P(lcdLine1, PSTR("%-11S%d.%02dV"),
                module[j].cycleCount > 0 ? PSTR("DETECTED") : PSTR("INSERT BAT"),
                (int)module[j].batteryVoltage,
                (int)(module[j].batteryVoltage * 100) % 100);
      break;

    case 1: // Barcode scan
      sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-SCAN BARCODE"));
      sprintf_P(lcdLine1, PSTR("           %d.%02dV"),
                (int)module[j].batteryVoltage,
                (int)(module[j].batteryVoltage * 100) % 100);
      break;

    case 2: // Charging
      sprintf_P(lcdLine0, PSTR("%d%-7S%02d:%02d:%02d"),
                j + 1, PSTR("-CHRG "),
                module[j].hours, module[j].minutes, module[j].seconds);
      sprintf_P(lcdLine1, PSTR("%d.%02dV  %02d%c %d.%02dV"),
                (int)module[j].batteryInitialVoltage,
                (int)(module[j].batteryInitialVoltage * 100) % 100,
                module[j].batteryCurrentTemp, 223, // ° symbol
                (int)module[j].batteryVoltage,
                (int)(module[j].batteryVoltage * 100) % 100);
      break;

    case 3: // Resistance
      sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-RESISTANCE"));
      sprintf_P(lcdLine1, PSTR("%-10S%04dm%c"),
                PSTR("MILLIOHMS"),
                (int)module[j].milliOhmsValue, 244); // Ω symbol
      break;

    case 4: // Rest
      sprintf_P(lcdLine0, PSTR("%d%-7S%02d:%02d:%02d"),
                j + 1, PSTR("-REST"),
                module[j].hours, module[j].minutes, module[j].seconds);
      sprintf_P(lcdLine1, PSTR("           %d.%02dV"),
                (int)module[j].batteryVoltage,
                (int)(module[j].batteryVoltage * 100) % 100);
      break;

    case 5: // Discharge
      sprintf_P(lcdLine0, PSTR("%d%-4S%d.%02dA %d.%02dV"),
                j + 1, PSTR("-DC"),
                (int)module[j].dischargeAmps,
                (int)(module[j].dischargeAmps * 100) % 100,
                (int)module[j].dischargeVoltage,
                (int)(module[j].dischargeVoltage * 100) % 100);
      sprintf_P(lcdLine1, PSTR("%02d:%02d:%02d %04dmAh"),
                module[j].hours, module[j].minutes, module[j].seconds,
                (int)module[j].dischargeMilliamps);
      break;

    case 6: // Recharge
      sprintf_P(lcdLine0, PSTR("%d%-7S%02d:%02d:%02d"),
                j + 1, PSTR("-RCHG "),
                module[j].hours, module[j].minutes, module[j].seconds);
      sprintf_P(lcdLine1, PSTR("%d.%02dV  %02d%c %d.%02dV"),
                (int)module[j].batteryInitialVoltage,
                (int)(module[j].batteryInitialVoltage * 100) % 100,
                module[j].batteryCurrentTemp, 223,
                (int)module[j].batteryVoltage,
                (int)(module[j].batteryVoltage * 100) % 100);
      break;

    case 7: // Finished / Fault
      switch (module[j].batteryFaultCode)
      {
        case 3: sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-FAULT HIGH OHM")); break;
        case 5: sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-FAULT LOW mAh")); break;
        case 7: sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-FAULT HIGH TMP")); break;
        case 9: sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-FAULT CHG TIME")); break;
        default:sprintf_P(lcdLine0, PSTR("%d%-15S"), j + 1, PSTR("-FINISHED")); break;
      }

      sprintf_P(lcdLine1, PSTR("%04dm%c   %04dmAh"),
                (int)module[j].milliOhmsValue, 244,
                (int)module[j].dischargeMilliamps);
      break;
  }

  lcd.setCursor(0, 0);
  lcd.print(lcdLine0);
  lcd.setCursor(0, 1);
  lcd.print(lcdLine1);
}
