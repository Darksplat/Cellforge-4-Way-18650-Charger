
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


#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Hardware.h"   // For ONE_WIRE_BUS_PIN

// ----------------- Config -----------------

constexpr uint8_t EXPECTED_SENSOR_COUNT   = 5;      // 4 battery + 1 ambient
constexpr float   TEMP_DELTA_THRESHOLD_C  = 2.5f;   // degrees above average

// ----------------- Globals -----------------

OneWire oneWire(ONE_WIRE_BUS_PIN);
DallasTemperature sensors(&oneWire);

// Arrays to hold device addresses
DeviceAddress tempSensorSerial[EXPECTED_SENSOR_COUNT];

// Detection bookkeeping
float baselineAverageTempC = 0.0f;
bool  sensorAssigned[EXPECTED_SENSOR_COUNT] = {false, false, false, false, false};
uint8_t sensorIndexForBattery[EXPECTED_SENSOR_COUNT]; // [0..3] batteries, [4] ambient
bool detectionComplete = false;

// ----------------- Helpers -----------------

void printDeviceAddress(const uint8_t* addr)
{
  for (uint8_t i = 0; i < 8; i++) {
    Serial.print("0x");
    if (addr[i] < 0x10) Serial.print("0");
    Serial.print(addr[i], HEX);
    if (i < 7) Serial.print(", ");
  }
}

uint8_t genericOneWireCount()
{
  uint8_t count = 0;
  uint8_t address[8];
  oneWire.reset_search();
  while (oneWire.search(address)) {
    count++;
  }
  return count;
}

// Compute a baseline average temperature across all sensors
void computeBaselineAverage()
{
  sensors.requestTemperatures();
  float sum = 0.0f;

  for (uint8_t i = 0; i < EXPECTED_SENSOR_COUNT; i++) {
    float t = sensors.getTempC(tempSensorSerial[i]);
    sum += t;
  }
  baselineAverageTempC = sum / EXPECTED_SENSOR_COUNT;

  Serial.print("Baseline average temperature: ");
  Serial.print(baselineAverageTempC, 2);
  Serial.println(" C");
}

// ----------------- Setup -----------------

void setup()
{
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ASCD Nano - DS18B20 Serial Detection (Guided)");
  Serial.println("------------------------------------------------");
  Serial.println("Preparation:");
  Serial.println(" 1. Connect all 5 DS18B20 sensors (4 battery + 1 ambient).");
  Serial.println(" 2. Ensure everything is at room temperature.");
  Serial.println(" 3. Do NOT heat any sensors yet.");
  Serial.println();
  Serial.println("When ready, we will:");
  Serial.println(" - Detect all OneWire devices.");
  Serial.println(" - Ask you to heat each battery sensor in turn.");
  Serial.println(" - Map physical sensors to: Battery 1-4 + Ambient.");
  Serial.println();

  // Start OneWire/Dallas
  sensors.begin();

  uint8_t found = sensors.getDeviceCount();
  Serial.print("DallasTemperature reports ");
  Serial.print(found);
  Serial.println(" device(s) on the bus.");

  if (found != EXPECTED_SENSOR_COUNT) {
    Serial.println();
    Serial.println("WARNING: Expected 5 sensors (4 battery + 1 ambient).");
    Serial.println("Attempting generic OneWire search...");
    uint8_t owCount = genericOneWireCount();
    Serial.print("Generic OneWire search found ");
    Serial.print(owCount);
    Serial.println(" device(s).");

    if (owCount != EXPECTED_SENSOR_COUNT) {
      Serial.println();
      Serial.println("ERROR: Sensor count mismatch.");
      Serial.println(" - Check wiring.");
      Serial.println(" - Confirm all 5 DS18B20 are connected.");
      Serial.println("Halting.");
      while (true) { /* forever */ }
    }
  }

  // Now actually get the addresses into tempSensorSerial[]
  oneWire.reset_search();
  uint8_t idx = 0;
  while (idx < EXPECTED_SENSOR_COUNT && oneWire.search(tempSensorSerial[idx])) {
    Serial.print("Found raw device ");
    Serial.print(idx);
    Serial.print(" address: ");
    printDeviceAddress(tempSensorSerial[idx]);
    Serial.println();
    sensors.setResolution(tempSensorSerial[idx], 9);  // 9-bit is fine
    idx++;
  }

  if (idx != EXPECTED_SENSOR_COUNT) {
    Serial.println();
    Serial.println("ERROR: Could not read all sensor addresses.");
    Serial.println("Halting.");
    while (true) { /* forever */ }
  }

  // Compute baseline average temp with all sensors at room temp
  Serial.println();
  Serial.println("Computing baseline average temperature...");
  computeBaselineAverage();

  Serial.println();
  Serial.println("DETECTION INSTRUCTIONS");
  Serial.println("----------------------");
  Serial.println("We will now detect which sensor is which by heating them one by one.");
  Serial.println("For each battery slot:");
  Serial.println(" 1. Wait for instructions 'Heat sensor for Battery X'.");
  Serial.println(" 2. Gently warm the matching probe (pinch it between fingers, etc.).");
  Serial.println(" 3. When you see a sensor jump > 2.5 C above baseline,");
  Serial.println("    it will be assigned to that battery.");
  Serial.println();
  Serial.println("Ambient sensor will be whichever sensor is left at the end.");
  Serial.println();
}

// ----------------- Loop -----------------

void loop()
{
  if (!detectionComplete) {
    // Detect sensors for Battery 1..4
    for (uint8_t battery = 0; battery < EXPECTED_SENSOR_COUNT - 1; ++battery) {
      if (sensorAssigned[battery]) {
        continue;
      }

      Serial.println();
      Serial.println("------------------------------------------------");
      Serial.print("Heat sensor for BATTERY ");
      Serial.print(battery + 1);
      Serial.println(" now!");
      Serial.println("Keep it warm until 'Detected Battery X' appears.");
      Serial.println("------------------------------------------------");

      bool thisBatteryDone = false;

      while (!thisBatteryDone) {
        sensors.requestTemperatures();

        for (uint8_t i = 0; i < EXPECTED_SENSOR_COUNT; i++) {
          if (sensorAssigned[i]) {
            continue; // skip already-assigned sensors
          }

          float t = sensors.getTempC(tempSensorSerial[i]);
          Serial.print("Sensor ");
          Serial.print(i);
          Serial.print(" Temp: ");
          Serial.print(t, 2);
          Serial.println(" C");

          if (t > baselineAverageTempC + TEMP_DELTA_THRESHOLD_C) {
            Serial.println();
            Serial.print("Detected Battery ");
            Serial.print(battery + 1);
            Serial.print(" as sensor index ");
            Serial.println(i);

            sensorAssigned[i] = true;
            sensorIndexForBattery[battery] = i;
            thisBatteryDone = true;
            break;
          }
        }

        Serial.println("Waiting 4 seconds before next temp sample...");
        delay(4000);
      }

      // After each battery detection, recompute baseline with remaining sensors if you like:
      // (Optional, often not necessary, but can be helpful if ambient conditions are drifting.)
      computeBaselineAverage();
    }

    // Remaining unassigned sensor is ambient
    Serial.println();
    Serial.println("------------------------------------------------");
    Serial.println("Detecting ambient sensor (the last unheated one)...");
    uint8_t ambientIndex = 255;
    for (uint8_t i = 0; i < EXPECTED_SENSOR_COUNT; i++) {
      if (!sensorAssigned[i]) {
        ambientIndex = i;
        break;
      }
    }

    if (ambientIndex == 255) {
      Serial.println("ERROR: Could not find an unassigned sensor for ambient.");
      Serial.println("Halting.");
      while (true) { /* forever */ }
    }

    sensorIndexForBattery[EXPECTED_SENSOR_COUNT - 1] = ambientIndex; // index 4

    Serial.print("Ambient sensor is index ");
    Serial.println(ambientIndex);

    detectionComplete = true;

    // --------------- Final output ---------------

    Serial.println();
    Serial.println("================================================");
    Serial.println(" DS18B20 ADDRESS MAPPING COMPLETE");
    Serial.println("================================================");
    Serial.println();
    Serial.println("Order: [Battery1, Battery2, Battery3, Battery4, Ambient]");
    Serial.println();
    Serial.println("Copy the following into your Temp_Sensor_Serials.h:");
    Serial.println();

    Serial.print("DeviceAddress tempSensorSerial[");
    Serial.print(EXPECTED_SENSOR_COUNT);
    Serial.println("] = {");

    for (uint8_t logicalIndex = 0; logicalIndex < EXPECTED_SENSOR_COUNT; ++logicalIndex) {
      uint8_t physicalIndex = sensorIndexForBattery[logicalIndex];

      Serial.print("  {");
      printDeviceAddress(tempSensorSerial[physicalIndex]);
      Serial.print("}");

      if (logicalIndex < EXPECTED_SENSOR_COUNT - 1) {
        Serial.println(",");
      } else {
        Serial.println();
      }
    }
    Serial.println("};");

    Serial.println();
    Serial.println("Finished. Reset the board to re-run detection.");
    Serial.println("================================================");
  }

  // Once complete, just idle; user can reset to run again.
  delay(1000);
}
