/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>

// =====================================================
// Timing and per-module variable initialisation
// =====================================================

void secondsTimer(byte j)
{
  unsigned long long longMilliSecondsCount =
    millis() - module[j].longMilliSecondsCleared;

  module[j].hours   = longMilliSecondsCount / (1000ULL * 60ULL * 60ULL);
  module[j].minutes =
    (longMilliSecondsCount % (1000ULL * 60ULL * 60ULL)) / (1000ULL * 60ULL);
  module[j].seconds =
    (longMilliSecondsCount % (1000ULL * 60ULL)) / 1000ULL;
}

void clearSecondsTimer(byte j)
{
  module[j].longMilliSecondsCleared = millis();
  module[j].seconds = 0;
  module[j].minutes = 0;
  module[j].hours   = 0;
}

void initializeVariables(byte j)
{
  // Reset per-cycle values
  module[j].batteryBarcode                 = false;
  module[j].insertData                     = false;
  module[j].tempMilliOhmsValue             = 0;
  module[j].milliOhmsValue                 = 0;

  module[j].intMilliSecondsCount           = 0;
  module[j].longMilliSecondsPreviousCount  = 0;
  module[j].longMilliSecondsPrevious       = 0;
  module[j].longMilliSecondsPassed         = 0;

  module[j].dischargeMilliamps              = 0.0;
  module[j].dischargeVoltage                = 0.0;
  module[j].dischargeAmps                   = 0.0;

  module[j].batteryFaultCode                = 0;
  module[j].batteryInitialTemp              = 0;
  module[j].batteryCurrentTemp              = 0;
  module[j].batteryHighestTemp              = 0;
}
