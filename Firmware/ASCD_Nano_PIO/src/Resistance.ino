
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
 * Internal resistance (milliOhms) measurement.
 */

byte milliOhms(byte j)
{
	float resistanceAmps       = 0.00;
	float voltageDrop          = 0.00;
	float batteryVoltageInput  = 0.00;
	float batteryShuntVoltage  = 0.00;

	digitalSwitch(module[j].dischargeMosfetPin, 0);
	batteryVoltageInput = readMux(module[j].batteryVolatgePin);

	digitalSwitch(module[j].dischargeMosfetPin, 1);
	batteryShuntVoltage = readMux(module[j].batteryVolatgePin);

	digitalSwitch(module[j].dischargeMosfetPin, 0);

	resistanceAmps = batteryShuntVoltage / settings.shuntResistor[j];
	voltageDrop    = batteryVoltageInput - batteryShuntVoltage;

	module[j].milliOhmsValue = ((voltageDrop / resistanceAmps) * 1000) + settings.offsetMilliOhms;

	if (module[j].milliOhmsValue > 9999)
	{
		module[j].milliOhmsValue = 9999;
	}

	return 1;
}
