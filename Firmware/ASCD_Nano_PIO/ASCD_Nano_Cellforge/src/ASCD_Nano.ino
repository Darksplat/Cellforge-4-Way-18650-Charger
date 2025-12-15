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

// ASCD_Nano.ino
// ASCD Nano 4x 18650 Charger / Discharger
// Main file: includes, globals, setup() and loop().

#include <Arduino.h>
#include <Wire.h>
#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#include "DebugConfig.h"
#include "Temp_Sensor_Serials.h"

// ----------------------
// Pins
// ----------------------

#define TEMPERATURE_PRECISION 9
#define ONE_WIRE_BUS 4 // Pin 4 Temperature Sensors

// 74HC595 shift register pins
const byte latchPin = 7;  // ST_CP
const byte clockPin = 8;  // SH_CP
const byte dataPin  = 6;  // DS

// Mux control pins
const byte S0 = 12;
const byte S1 = 11;
const byte S2 = 10;
const byte S3 = 9;

// Mux SIG pin (Analog A0)
const byte SIG = 14;

// Button pin (Analog A1)
const byte BTN = 15;

// Buzzer pin (Analog A2)
// const byte BUZZ = 5; // PCB Version 1.1
const byte BUZZ = 16;     // PCB Version 1.11

// Fan pin (PWM, Digital 5)
const byte FAN = 5;       // PCB Version 1.11+ only

// ----------------------
// Objects
// ----------------------

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD at address 0x27, 16x2
OneWire oneWire(ONE_WIRE_BUS);      // OneWire bus for DS18B20 sensors
DallasTemperature sensors(&oneWire);
SoftwareSerial ESP8266(3, 2);       // RX, TX

// ----------------------
// Settings struct
// ----------------------

typedef struct
{
  const float shuntResistor[4]           = {3.3, 3.3, 3.3, 3.3};
  const float chargeLedPinMidVolatge[4]  = {1.8, 1.8, 1.85, 1.85};
  const float referenceVoltage           = 5.02;
  const float defaultBatteryCutOffVoltage= 2.8;
  const byte  restTimeMinutes            = 1;
  const int   lowMilliamps               = 1000;
  const int   highMilliOhms              = 500;
  const int   offsetMilliOhms            = 0;
  const byte  chargingTimeout            = 8;
  const byte  tempThreshold              = 7;
  const byte  tempMaxThreshold           = 20;
  const float batteryVolatgeLeak         = 0.50;
  const byte  moduleCount                = 4;
  const byte  screenTime                 = 4;
  const int   dischargeReadInterval      = 5000;
  const float storageChargeVoltage       = 0.00;
  const byte  pwmFanMinStart             = 115;   // Minimum PWM for fan start
} CustomSettings;

CustomSettings settings;

// ----------------------
// Module struct
// ----------------------

typedef struct
{
  // Pin Definitions (mux patterns)
  const bool batteryVolatgePin[4];
  const bool batteryVolatgeDropPin[4];
  const bool chargeLedPin[4];
  const byte chargeMosfetPin;
  const byte dischargeMosfetPin;

  // Timer
  unsigned long longMilliSecondsCleared;
  byte seconds;
  byte minutes;
  byte hours;

  // Module Cycle
  byte cycleCount;
  bool batteryBarcode;
  bool insertData;
  byte cycleState;
  byte batteryFaultCode;

  // Voltage Readings
  float batteryInitialVoltage;
  float batteryVoltage;

  // Temperature Readings
  byte batteryInitialTemp;
  byte batteryHighestTemp;
  byte batteryCurrentTemp;
  byte tempCount;

  // Milli Ohms
  float tempMilliOhmsValue;
  float milliOhmsValue;

  // Discharge
  int intMilliSecondsCount;
  unsigned long longMilliSecondsPreviousCount;
  unsigned long longMilliSecondsPrevious;
  unsigned long longMilliSecondsPassed;
  float dischargeMilliamps;
  float dischargeVoltage;
  float dischargeAmps;
  int dischargeMinutes;
  bool pendingDischargeRecord;
} Modules;

// Module pin configuration for 4 slots
Modules module[4] =
{
  {{1, 1, 0, 1}, {1, 1, 1, 1}, {0, 1, 0, 1}, 0, 1},
  {{1, 0, 0, 1}, {0, 1, 1, 1}, {0, 0, 0, 1}, 2, 3},
  {{1, 1, 1, 0}, {1, 0, 1, 1}, {0, 1, 1, 0}, 4, 5},
  {{1, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 1, 0}, 6, 7}
};

// ----------------------
// Global state
// ----------------------

byte ambientTemperature = 0;
bool  buttonPressed     = false;
bool  readSerialResponse= false;
char  serialSendString[400];
byte  countSerialSend   = 0;
bool  soundBuzzer       = false;

// ----------------------
// Forward declarations
// (functions are actually defined in the other .ino tabs)
// ----------------------

// Buzzer.ino
void buzzer();

// FanController.ino
void fanController();

// SerialComm.ino
void sendSerial();
void readSerial();
void returnCodes(int codeID);

// Button.ino
void button();

// LCD_UI.ino
void cycleStateLCD();
void cycleStateLCDOutput(byte j);

// Timing.ino
void secondsTimer(byte j);
void clearSecondsTimer(byte j);
void initializeVariables(byte j);

// StateMachine.ino
void cycleStateValues();

// Discharge.ino
bool dischargeCycle(byte j);

// Charge.ino
bool chargeCycle(byte j);

// Resistance.ino
byte milliOhms(byte j);

// Temperature.ino
byte getTemperature(byte j);
byte processTemperature(byte j);
void getAmbientTemperature();

// IOUtils.ino
bool  batteryCheck(byte j);
void  digitalSwitch(byte j, bool value);
float readMux(const bool inputArray[]);

// ----------------------
// setup() and loop()
// ----------------------

void setup()
{
  // Shift register pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin,  OUTPUT);
  digitalWrite(latchPin, LOW);

  // MUX initialisation
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(S0, LOW);
  digitalWrite(S1, LOW);
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  // Button
  pinMode(BTN, INPUT);

  // Buzzer
  pinMode(BUZZ, OUTPUT);
  digitalWrite(BUZZ, HIGH);
  delay(50);
  digitalWrite(BUZZ, LOW);

  // Fan
  pinMode(FAN, OUTPUT);

  // Serial for debug + ESP link
  DBG_BEGIN(115200);
  Serial.setTimeout(5);

  // SoftwareSerial to ESP8266
  ESP8266.begin(57600);
  ESP8266.setTimeout(5);

  // LCD startup
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("ASCD NANO V1.0.2"));
  lcd.setCursor(0, 1);
  lcd.print(F("Init TP5100....."));

  // Initial module / MOSFET sequence & mux pre-pull down
  for (byte i = 0; i < settings.moduleCount; i++)
  {
    digitalWrite(FAN, HIGH); // Fan on during initialisation

    digitalSwitch(module[i].chargeMosfetPin, 1);
    delay(500);
    digitalSwitch(module[i].chargeMosfetPin, 0);
    delay(500);

    // Read each battery voltage input to discharge stray charge
    readMux(module[i].batteryVolatgePin);

    digitalSwitch(module[i].dischargeMosfetPin, 1);
    digitalWrite(FAN, LOW); // Fan off
    delay(500);
    digitalSwitch(module[i].dischargeMosfetPin, 0);
    delay(500);
  }

  lcd.setCursor(0, 1);
  lcd.print(F("Starting........"));

  // Start DallasTemperature library
  sensors.begin();

  lcd.clear();
}

void loop()
{
  if (readSerialResponse == true)
  {
    readSerial();
  }

  // Timers using millis()
  static long buttonMillis;
  static long cycleStateValuesMillis;
  static long sendSerialMillis;
  static long buzzerMillis;
  long currentMillis = millis();

  // Poll button every 2 ms
  if (currentMillis - buttonMillis >= 2)
  {
    button();
    buttonMillis = currentMillis;
  }

  currentMillis = millis();
  // Buzzer update every 50 ms
  if (currentMillis - buzzerMillis >= 50)
  {
    buzzer();
    buzzerMillis = currentMillis;
  }

  currentMillis = millis();
  // Core cycle logic every 1 second
  if (currentMillis - cycleStateValuesMillis >= 1000)
  {
    cycleStateValues();
    cycleStateValuesMillis = currentMillis;
  }

  currentMillis = millis();
  // Send serial every 4 seconds
  if (currentMillis - sendSerialMillis >= 4000)
  {
    if (readSerialResponse == false || countSerialSend > 5)
    {
      sendSerial();
      countSerialSend = 0;
    }
    else
    {
      countSerialSend++;
    }
    sendSerialMillis = currentMillis;
  }
}
