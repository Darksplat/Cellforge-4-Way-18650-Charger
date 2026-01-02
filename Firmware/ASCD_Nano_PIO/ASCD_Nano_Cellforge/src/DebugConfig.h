/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

// DebugConfig.h
// Global debug configuration and helper macros.

#ifndef DEBUG_CONFIG_H
#define DEBUG_CONFIG_H

// =====================================================
// DEBUG CONTROL
// =====================================================
//
// Set DEBUG_ENABLED to 1 to enable serial debug output.
// Set DEBUG_ENABLED to 0 to completely disable all debug
// printing (recommended for normal operation / release).
//
// You can also override this value using compiler flags
// (e.g. PlatformIO or build scripts) if required.
//
// Examples:
//   #define DEBUG_ENABLED 1   // Debug ON
//   #define DEBUG_ENABLED 0   // Debug OFF
//

#ifndef DEBUG_ENABLED
  #define DEBUG_ENABLED 0   // <-- CHANGE THIS TO 0 TO TURN OFF DEBUG OUTPUT
#endif

// =====================================================
// Debug macros
// =====================================================

#if DEBUG_ENABLED
  #define DBG_BEGIN(baud)   Serial.begin(baud)
  #define DBG_PRINT(x)      Serial.print(x)
  #define DBG_PRINTLN(x)    Serial.println(x)
#else
  #define DBG_BEGIN(baud)   do {} while (0)
  #define DBG_PRINT(x)      do {} while (0)
  #define DBG_PRINTLN(x)    do {} while (0)
#endif

#endif // DEBUG_CONFIG_H
