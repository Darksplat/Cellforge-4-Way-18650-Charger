
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

// DebugConfig.h
// Global debug configuration and helper macros.

// Set to 1 to enable debug output, 0 to disable
#define DEBUG_ENABLED 1

#if DEBUG_ENABLED
  #define DBG_BEGIN(baud)   Serial.begin(baud)
  #define DBG_PRINT(x)      Serial.print(x)
  #define DBG_PRINTLN(x)    Serial.println(x)
#else
  #define DBG_BEGIN(baud)   // no-op
  #define DBG_PRINT(x)      // no-op
  #define DBG_PRINTLN(x)    // no-op
#endif
