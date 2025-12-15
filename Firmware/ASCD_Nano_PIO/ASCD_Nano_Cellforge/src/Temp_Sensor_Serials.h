
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

// Temp_Sensor_Serials.h
// Dallas Temperature Sensor serial numbers.

#ifndef TEMP_SENSOR_SERIALS_H
#define TEMP_SENSOR_SERIALS_H

#include <DallasTemperature.h>  // Defines DeviceAddress

// Replace these with your own sensor addresses if you change hardware.
DeviceAddress tempSensorSerial[5] =
{
  {0x28, 0x55, 0xE9, 0x79, 0x97, 0x05, 0x03, 0x86}, // Module 0
  {0x28, 0x90, 0x09, 0x79, 0x97, 0x02, 0x03, 0x46}, // Module 1
  {0x28, 0xCF, 0x2B, 0x79, 0x97, 0x05, 0x03, 0x59}, // Module 2
  {0x28, 0x4B, 0x6C, 0x79, 0x97, 0x04, 0x03, 0x13}, // Module 3
  {0x28, 0xC8, 0x9C, 0x79, 0x97, 0x05, 0x03, 0xC2}  // Ambient
};

#endif // TEMP_SENSOR_SERIALS_H
