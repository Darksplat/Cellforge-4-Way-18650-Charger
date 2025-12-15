// ASDC Nano 4x Arduino Charger / Discharger
// ASCD Nano Get DS18B20 Serials and Test Sensors
// -----------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Copyright 2019 - Under creative commons license 3.0:
// Modified by Jeremy Younger 29/10/2023
//
// This software is furnished "as is," without technical support, and with no
// warranty, express or implied, as to its usefulness for any purpose.
//
// @brief
// ASDC Nano 4x Arduino Charger / Discharger
// Code for retrieving Dallas DS18B20 sensor serials
// Version 1.0.0
//
// @author Email: info@vortexit.co.nz
//       Web: www.vortexit.co.nz
//       Modified by email: darksplat@gmail.com
//       GitHub Repository: https://github.com/Darksplat

#include <OneWire.h>
#include <DallasTemperature.h>

// Create an array of OneWire instances for each pin
OneWire oneWire[] = {OneWire(2), OneWire(3), OneWire(4), OneWire(5), OneWire(6), OneWire(7), OneWire(8), OneWire(9), OneWire(10), OneWire(11), OneWire(12)};

// Create an array of DallasTemperature instances for each OneWire instance
DallasTemperature sensors[] = {DallasTemperature(&oneWire[0]), DallasTemperature(&oneWire[1]), DallasTemperature(&oneWire[2]), DallasTemperature(&oneWire[3]), DallasTemperature(&oneWire[4]), DallasTemperature(&oneWire[5]), DallasTemperature(&oneWire[6]), DallasTemperature(&oneWire[7]), DallasTemperature(&oneWire[8]), DallasTemperature(&oneWire[9]), DallasTemperature(&oneWire[10])};

int currentSensor = 0; // To keep track of the current sensor being tested

// const int buttonPin = 13; // Change to the appropriate button pin

void setup()
{
  Serial.begin(115200);
  Serial.println("Start oneWireSearch.ino");

  for (int i = 0; i < 11; i++) {
    sensors[i].begin();
    sensors[i].setResolution(9);  // You can change the resolution (9-12 bits) if needed
  }

  // pinMode(buttonPin, INPUT_PULLUP); // Initialize the button pin
}

void loop()
{
  // Check if we should test a sensor
  // if (digitalRead(buttonPin) == LOW)
  {
    testSensor(currentSensor);
    currentSensor++; // Move on to the next sensor

    // If we've tested all sensors, reset to the first sensor and add a 5-second delay
    if (currentSensor >= 11)
    {
      currentSensor = 0;
      delay(5000); // 5-second delay
    }
  }
}

void testSensor(int sensorIndex)
{
  Serial.print("Testing sensor on pin ");
  Serial.println(sensorIndex + 2);

  DeviceAddress tempDeviceAddress[10];  // Assuming a maximum of 10 devices on a single pin
  int deviceCount = sensors[sensorIndex].getDeviceCount();

  if (deviceCount > 0)
  {
    sensors[sensorIndex].requestTemperatures();

    Serial.println("------------------------------------------------------------------------------------------");
    Serial.println("| Device # |  Address                                          |  Temperature (Celsius)  |");
    Serial.println("----------------------------------------------------------------------------------------");

    for (int i = 0; i < deviceCount; i++)
    {
      sensors[sensorIndex].getAddress(tempDeviceAddress[i], i);
      float tempC = sensors[sensorIndex].getTempC(tempDeviceAddress[i]);
      Serial.print("|   ");
      Serial.print(i);
      Serial.print("      |   ");
      printAddress(tempDeviceAddress[i]);
      Serial.print("  |                   ");
      Serial.print(tempC, 2);
      Serial.println(" |");
    }

    Serial.println("------------------------------------------------------------------------------------------");

    // Now, wait for operator acknowledgment
    Serial.println("Did the temperature go up after 5 seconds of the heat source being applied?");
    Serial.println("For acknowledgment of the change, a finger being held on top of the sensor should be sufficient.");
    Serial.println();

    // Print the sensor addresses separately
    printSensorAddresses(sensorIndex, tempDeviceAddress, deviceCount);

    // Wait for the button press to acknowledge the test
    // while (digitalRead(buttonPin) == HIGH)
    {
      // You can add some delay or other logic here if needed
    }
  }
}

void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    Serial.print("0x");
    if (deviceAddress[i] < 0x10)
      Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
    if (i < 7)
      Serial.print(", ");
  }
}

void printSensorAddresses(int sensorIndex, DeviceAddress* addresses, int count)
{
  Serial.println("Sensor Addresses for Sensor on Pin " + String(sensorIndex + 2));
  Serial.println("Copy the sensor addresses into the Temp-Sensor-Serials.h tab. of the main code");
  Serial.println("{");
  for (int i = 0; i < count; i++)
  {
    Serial.print("{");
    for (int j = 0; j < 8; j++)
    {
      Serial.print("0x");
      if (addresses[i][j] < 0x10)
        Serial.print("0");
      Serial.print(addresses[i][j], HEX);
      if (j < 7)
        Serial.print(", ");
    }
    Serial.print("}");
    if (i < count - 1)
      Serial.print(",");
    Serial.println();
  }
  Serial.println("}");
}
