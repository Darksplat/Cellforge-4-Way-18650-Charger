
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
 * Discharge cycle handler for a module.
 * Integrates current over time to estimate mAh.
 */

bool dischargeCycle(byte j)
{
	float batteryShuntVoltage = 0.00;
	module[j].intMilliSecondsCount += (millis() - module[j].longMilliSecondsPreviousCount);
	module[j].longMilliSecondsPreviousCount = millis();

	// Take reading every interval or on first run
	if (module[j].intMilliSecondsCount >= settings.dischargeReadInterval || module[j].dischargeAmps == 0)
	{
		module[j].dischargeVoltage = readMux(module[j].batteryVolatgePin);
		batteryShuntVoltage        = readMux(module[j].batteryVolatgeDropPin);

		if (module[j].dischargeVoltage >= settings.defaultBatteryCutOffVoltage)
		{
			digitalSwitch(module[j].dischargeMosfetPin, 1); // Turn on discharge MOSFET
			module[j].dischargeAmps = (module[j].dischargeVoltage - batteryShuntVoltage) / settings.shuntResistor[j];

			module[j].longMilliSecondsPassed = millis() - module[j].longMilliSecondsPrevious;
			module[j].dischargeMilliamps +=
				(module[j].dischargeAmps * 1000.0) * (module[j].longMilliSecondsPassed / 3600000.0);
			module[j].longMilliSecondsPrevious = millis();
		}

		module[j].intMilliSecondsCount = 0;

		// Below cutoff voltage: stop discharge
		if (module[j].dischargeVoltage < settings.defaultBatteryCutOffVoltage)
		{
			digitalSwitch(module[j].dischargeMosfetPin, 0);
			return true;
		}
	}
	return false;
}
