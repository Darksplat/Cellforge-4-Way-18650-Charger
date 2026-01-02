/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

// Temp_Sensor_Serials.h
// Dallas Temperature Sensor serial numbers.
//
// Index mapping:
//   0–3 : Battery modules U1–U4
//   4   : Ambient sensor U5 (primary)
//   5   : Ambient sensor U13 (optional)

#ifndef TEMP_SENSOR_SERIALS_H
#define TEMP_SENSOR_SERIALS_H

#include <DallasTemperature.h>  // Defines DeviceAddress

#define TEMP_SENSOR_COUNT 6

static const DeviceAddress tempSensorSerial[TEMP_SENSOR_COUNT] =
{
  // Module temperature sensors (U1–U4)
  {0x28, 0x55, 0xE9, 0x79, 0x97, 0x05, 0x03, 0x86}, // U1 – Module 0
  {0x28, 0x90, 0x09, 0x79, 0x97, 0x02, 0x03, 0x46}, // U2 – Module 1
  {0x28, 0xCF, 0x2B, 0x79, 0x97, 0x05, 0x03, 0x59}, // U3 – Module 2
  {0x28, 0x4B, 0x6C, 0x79, 0x97, 0x04, 0x03, 0x13}, // U4 – Module 3

  // Ambient sensors
  {0x28, 0xC8, 0x9C, 0x79, 0x97, 0x05, 0x03, 0xC2}, // U5 – Ambient (primary)
  {0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // U13 – Ambient (optional)
};

#endif // TEMP_SENSOR_SERIALS_H
