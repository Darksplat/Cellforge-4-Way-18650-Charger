/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Copyright 2018 - Under creative commons license 3.0:
//
// Modified by Jeremy Younger @darksplat on 06/12/2025
// https://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
*/

#include <Arduino.h>
#include <Wire.h>

#include "Pins.h"              // MUST be first custom header
#include "DebugConfig.h"
#include "Temp_Sensor_Serials.h"
#include "Secrets.h"

#include <OneWire.h>
#include <LiquidCrystal_I2C.h>
#include <DallasTemperature.h>
// #include <SoftwareSerial.h>   // Disabled for Nano ESP32




// ----------------------
// Objects
// ----------------------

LiquidCrystal_I2C lcd(0x27, 16, 2);
OneWire oneWire(PIN_TMP);
DallasTemperature sensors(&oneWire);

// SoftwareSerial ESP8266(3, 2); // RX, TX – disabled for ESP32


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
  const byte  pwmFanMinStart             = 115;
} CustomSettings;

CustomSettings settings;


// ----------------------
// Module struct (UNCHANGED for now)
// ----------------------

typedef struct
{
  const bool batteryVolatgePin[4];
  const bool batteryVolatgeDropPin[4];
  const bool chargeLedPin[4];
  const byte chargeMosfetPin;
  const byte dischargeMosfetPin;

  unsigned long longMilliSecondsCleared;
  byte seconds;
  byte minutes;
  byte hours;

  byte cycleCount;
  bool batteryBarcode;
  bool insertData;
  byte cycleState;
  byte batteryFaultCode;

  float batteryInitialVoltage;
  float batteryVoltage;

  byte batteryInitialTemp;
  byte batteryHighestTemp;
  byte batteryCurrentTemp;
  byte tempCount;

  float tempMilliOhmsValue;
  float milliOhmsValue;

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

Modules module[4] =
{
  {{1,1,0,1}, {1,1,1,1}, {0,1,0,1}, 0, 1},
  {{1,0,0,1}, {0,1,1,1}, {0,0,0,1}, 2, 3},
  {{1,1,1,0}, {1,0,1,1}, {0,1,1,0}, 4, 5},
  {{1,0,1,0}, {0,0,1,1}, {0,0,1,0}, 6, 7}
};


// ----------------------
// Global state
// ----------------------

byte ambientTemperature = 0;
bool buttonPressed = false;
bool readSerialResponse = false;
char serialSendString[400];
byte countSerialSend = 0;
bool soundBuzzer = false;


// ----------------------
// Forward declarations
// ----------------------

void buzzer();
void fanController();
void sendSerial();
void readSerial();
void returnCodes(int codeID);
void button();
void cycleStateLCD();
void cycleStateLCDOutput(byte j);
void secondsTimer(byte j);
void clearSecondsTimer(byte j);
void initializeVariables(byte j);
void cycleStateValues();
bool dischargeCycle(byte j);
bool chargeCycle(byte j);
byte milliOhms(byte j);
byte getTemperature(byte j);
byte processTemperature(byte j);
void getAmbientTemperature();
bool batteryCheck(byte j);
void digitalSwitch(byte j, bool value);
float readMux(const bool inputArray[]);   // to be replaced later


// ----------------------
// setup()
// ----------------------

void setup()
{
  // Shift register
  pinMode(PIN_SR_DATA, OUTPUT);
  pinMode(PIN_SR_LATCH, OUTPUT);
  pinMode(PIN_SR_CLOCK, OUTPUT);
  digitalWrite(PIN_SR_LATCH, LOW);

  // TP5100 charge status inputs
  pinMode(PIN_CD1, INPUT);
  pinMode(PIN_CD2, INPUT);
  pinMode(PIN_CD3, INPUT);
  pinMode(PIN_CD4, INPUT);

  // Button
  pinMode(PIN_BTN, INPUT);

  // Buzzer
  pinMode(PIN_BUZZ, OUTPUT);
  digitalWrite(PIN_BUZZ, HIGH);
  delay(50);
  digitalWrite(PIN_BUZZ, LOW);

  // Fan
  pinMode(PIN_FAN, OUTPUT);
  ledcSetup(FAN_PWM_CHANNEL, FAN_PWM_FREQ, FAN_PWM_RES);
  ledcAttachPin(PIN_FAN, FAN_PWM_CHANNEL);
  ledcWrite(FAN_PWM_CHANNEL, 0); // fan off


  // Debug serial
  DBG_BEGIN(115200);
  Serial.setTimeout(5);

  // LCD startup
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(F("ASCD NANO ESP32"));
  lcd.setCursor(0, 1);
  lcd.print(F("Init TP5100....."));

  // Initial module sequence
  for (byte i = 0; i < settings.moduleCount; i++)
  {
    digitalWrite(PIN_FAN, HIGH);

    digitalSwitch(module[i].chargeMosfetPin, 1);
    delay(500);
    digitalSwitch(module[i].chargeMosfetPin, 0);
    delay(500);

    // Legacy stray charge discharge (to be replaced with ADS read later)
    readMux(module[i].batteryVolatgePin);

    digitalSwitch(module[i].dischargeMosfetPin, 1);
    digitalWrite(PIN_FAN, LOW);
    delay(500);
    digitalSwitch(module[i].dischargeMosfetPin, 0);
    delay(500);
  }

  lcd.setCursor(0, 1);
  lcd.print(F("Starting........"));

  sensors.begin();
  lcd.clear();
}


// ----------------------
// loop()
// ----------------------

void loop()
{
  if (readSerialResponse)
  {
    readSerial();
  }

  static long buttonMillis;
  static long cycleStateValuesMillis;
  static long sendSerialMillis;
  static long buzzerMillis;

  long currentMillis = millis();

  if (currentMillis - buttonMillis >= 2)
  {
    button();
    buttonMillis = currentMillis;
  }

  if (currentMillis - buzzerMillis >= 50)
  {
    buzzer();
    buzzerMillis = currentMillis;
  }

  if (currentMillis - cycleStateValuesMillis >= 1000)
  {
    cycleStateValues();
    cycleStateValuesMillis = currentMillis;
  }

  if (currentMillis - sendSerialMillis >= 4000)
  {
    if (!readSerialResponse || countSerialSend > 5)
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
