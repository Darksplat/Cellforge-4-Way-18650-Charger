#pragma once

// =====================================================
// Pins.h
// Central pin definitions for ASCD Nano ESP32
// Hardware: Arduino Nano ESP32
// Schematic: Rev 2.0 (ADS1115, no analog mux)
// =====================================================

// =====================================================
// ESP32 Fan PWM (LEDC)
// =====================================================
#define FAN_PWM_CHANNEL   0
#define FAN_PWM_FREQ      25000   // 25 kHz (quiet fan)
#define FAN_PWM_RES       8        // 8-bit resolution

// ----------------------
// I2C (shared bus)
// ----------------------
static const uint8_t PIN_SDA = A4;
static const uint8_t PIN_SCL = A5;


// ----------------------
// 74HC595 Shift Register
// ----------------------
static const uint8_t PIN_SR_DATA = D6;   // DS
static const uint8_t PIN_SR_LATCH = D7;  // ST/CP
static const uint8_t PIN_SR_CLOCK = D8;  // SH/CP


// ----------------------
// TP5100 Charge Status Inputs
// (one per slot, no mux)
// ----------------------
static const uint8_t PIN_CD1 = D12;
static const uint8_t PIN_CD2 = D11;
static const uint8_t PIN_CD3 = D10;
static const uint8_t PIN_CD4 = D9;


// ----------------------
// Fan + Temperature
// ----------------------
static const uint8_t PIN_FAN = D5;   // PWM fan control
static const uint8_t PIN_TMP = D4;   // OneWire temperature bus


// ----------------------
// Button + Buzzer
// ----------------------
static const uint8_t PIN_BTN  = A1;
static const uint8_t PIN_BUZZ = A2;


// ----------------------
// ADS1115 I2C Addresses
// ----------------------
// Battery voltages ADS1115 (ADDR = GND)
static const uint8_t ADS_BAT_ADDR = 0x48;

// Load voltages ADS1115 (ADDR = 3V3)
static const uint8_t ADS_LOAD_ADDR = 0x49;


// ----------------------
// ADS1115 Channel Mapping
// ----------------------
// Battery voltages
static const uint8_t ADS_BV1 = 0; // AIN0
static const uint8_t ADS_BV2 = 1; // AIN1
static const uint8_t ADS_BV3 = 2; // AIN2
static const uint8_t ADS_BV4 = 3; // AIN3

// Load voltages
static const uint8_t ADS_RV1 = 0; // AIN0
static const uint8_t ADS_RV2 = 1; // AIN1
static const uint8_t ADS_RV3 = 2; // AIN2
static const uint8_t ADS_RV4 = 3; // AIN3
