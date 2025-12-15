
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

/**
 * Helper functions for muxed analog readings and shift-register IO.
 */

bool batteryCheck(byte j)
{
	module[j].batteryVoltage = readMux(module[j].batteryVolatgePin);
	if (module[j].batteryVoltage <= settings.batteryVolatgeLeak)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void digitalSwitch(byte j, bool value)
{
	byte baseTwo        = 1;
	byte eightBitDecimal = 0;
	static bool digitalPinsState[8]; // Shift register outputs (Q0..Q7)

	digitalPinsState[j] = value;

	for (byte i = 0; i < 8; i++)
	{
		if (digitalPinsState[i] == 1)
			eightBitDecimal += baseTwo;
		baseTwo *= 2;
	}

	// Latch low
	digitalWrite(latchPin, LOW);
	// Shift out new states
	shiftOut(dataPin, clockPin, MSBFIRST, eightBitDecimal);
	// Latch high
	digitalWrite(latchPin, HIGH);
}

float readMux(const bool inputArray[])

{
	const byte controlPin[] = {S0, S1, S2, S3};

	// Set mux control lines
	for (byte i = 0; i < 4; i++)
	{
		digitalWrite(controlPin[i], inputArray[i]);
	}

	// Read SIG 10x and average
	float batterySampleVoltage = 0.00;
	for (byte i = 0; i < 10; i++)
	{
		batterySampleVoltage += analogRead(SIG);
	}
	batterySampleVoltage /= 10.0;

	// Convert ADC value to voltage
	return batterySampleVoltage * settings.referenceVoltage / 1023.0;
}
