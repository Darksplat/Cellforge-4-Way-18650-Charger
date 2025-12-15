
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
// @author Email: darksplat@gmail.com
//       Web: www.darksplat.com
*/

/**
 * Temperature processing for modules and ambient.
 */

byte processTemperature(byte j)
{
	module[j].batteryCurrentTemp = getTemperature(j);

	// Track highest temp (except invalid 99)
	if (module[j].batteryCurrentTemp > module[j].batteryHighestTemp &&
	    module[j].batteryCurrentTemp != 99)
	{
		module[j].batteryHighestTemp = module[j].batteryCurrentTemp;
	}

	if ((module[j].batteryCurrentTemp - ambientTemperature) > settings.tempThreshold &&
	    module[j].batteryCurrentTemp != 99)
	{
		if ((module[j].batteryCurrentTemp - ambientTemperature) > settings.tempMaxThreshold)
		{
			// Above maximum threshold = fault
			return 2;
		}
		else
		{
			// Above warning threshold (no current action)
			return 1;
		}
	}
	else
	{
		// Within safe range
		return 0;
	}
}

byte getTemperature(byte j)
{
	if (module[j].tempCount > 16 || module[j].batteryCurrentTemp == 0 || module[j].batteryCurrentTemp == 99)
	{
		module[j].tempCount = 0;
		sensors.requestTemperaturesByAddress(tempSensorSerial[j]);
		float tempC = sensors.getTempC(tempSensorSerial[j]);

		if (tempC > 99 || tempC < 0)
		{
			tempC = 99;
			if (module[j].batteryCurrentTemp != 99)
			{
				tempC = module[j].batteryCurrentTemp;
			}
		}
		return (int)tempC;
	}
	else
	{
		module[j].tempCount++;
		return module[j].batteryCurrentTemp;
	}
}

void getAmbientTemperature()
{
	static byte ambientTempCount;

	if (ambientTempCount > 16 || ambientTemperature == 0 || ambientTemperature == 99)
	{
		ambientTempCount = 0;
		sensors.requestTemperaturesByAddress(tempSensorSerial[4]);
		float tempC = sensors.getTempC(tempSensorSerial[4]);

		if (tempC > 99 || tempC < 0)
		{
			tempC = 99;
			if (ambientTemperature != 99)
			{
				tempC = ambientTemperature;
			}
		}
		ambientTemperature = tempC;
	}
	else
	{
		ambientTempCount++;
	}
}
