
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
 * Core state machine for each module.
 * Called once per second from loop().
 *
 * Handles:
 *  - Battery presence checking
 *  - Barcode phase
 *  - Charge, rest, discharge, recharge
 *  - Temperature & fault logic
 *  - Serial telemetry & LCD update
 *  - Fan control
 */

	// --- existing implementation from your original code ---
	// (copy your full cycleStateValues() function body here unchanged)
	//  ... (for brevity, not repeated in this snippet)

void cycleStateValues()
{
	strcpy(serialSendString, "");
	getAmbientTemperature();
	sprintf_P(serialSendString + strlen(serialSendString), PSTR("&AT=%d"), ambientTemperature);
	for (byte i = 0; i < settings.moduleCount; i++)
	{
		switch (module[i].cycleState)
		{
		case 0: // Check Battery Voltage
			if (batteryCheck(i))
				module[i].cycleCount++;
			if (module[i].cycleCount == 5)
			{
				initializeVariables(i);
				module[i].batteryCurrentTemp = getTemperature(i);
				module[i].batteryInitialTemp = module[i].batteryCurrentTemp;
				module[i].batteryHighestTemp = module[i].batteryCurrentTemp;
				clearSecondsTimer(i);
				module[i].batteryVoltage = readMux(module[i].batteryVolatgePin); // Get battery voltage for Charge Cycle
				module[i].batteryInitialVoltage = module[i].batteryVoltage;
				module[i].cycleState = 1; // Check Battery Voltage Completed set cycleState to Get Battery Barcode
				module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
			}
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=0"), i);
			break;
		case 1:																 // Battery Barcode
			module[i].batteryVoltage = readMux(module[i].batteryVolatgePin); // Get battery voltage
			if (module[i].batteryBarcode == true)
			{
				clearSecondsTimer(i);
				module[i].batteryInitialVoltage = module[i].batteryVoltage; // Reset Initial voltage
				module[i].cycleState = 2;									// Get Battery Barcode Completed set cycleState to Charge Battery
			}
			//Check if battery has been removed
			if (!batteryCheck(i))
				module[i].cycleCount++;
			if (module[i].cycleCount == 5)
			{
				module[i].cycleState = 0; // Completed and Battery Removed set cycleState to Check Battery Voltage
				module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
			}
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=1"), i);
			break;
		case 2: // Charge Battery
			//Serial.println(readMux(module[i].chargeLedPin));
			module[i].batteryVoltage = readMux(module[i].batteryVolatgePin); // Get battery voltage
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=2&TI%d=%d&IT%d=%d&IV%d=%d.%02d&CT%d=%d&CV%d=%d.%02d&HT%d=%d"), i, i, (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)), i, module[i].batteryInitialTemp, i, (int)module[i].batteryInitialVoltage, (int)(module[i].batteryInitialVoltage * 100) % 100, i, module[i].batteryCurrentTemp, i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100, i, module[i].batteryHighestTemp);
			if (processTemperature(i) == 2)
			{
				//Battery Temperature is >= MAX Threshold considered faulty
				digitalSwitch(module[i].chargeMosfetPin, 0); // Turn off TP5100
				module[i].batteryFaultCode = 7;				 // Set the Battery Fault Code to 7 High Temperature
				if (module[i].insertData == true)
				{
					clearSecondsTimer(i);
					module[i].insertData = false;
					module[i].cycleState = 7; // Temperature is to high. Battery is considered faulty set cycleState to Completed
					module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
				}
				sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
			}
			else
			{
				digitalSwitch(module[i].chargeMosfetPin, 1); // Turn on TP5100
				module[i].cycleCount = module[i].cycleCount + chargeCycle(i);
				if (module[i].cycleCount >= 10)
				{
					digitalSwitch(module[i].chargeMosfetPin, 0); // Turn off TP5100
					if (module[i].insertData == true)
					{
						// clearSecondsTimer(i);
						module[i].insertData = false;
						module[i].cycleState = 3; // Charge Battery Completed set cycleState to Check Battery Milli Ohms
						module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
					}
					sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
				}
			}
			if (module[i].hours == settings.chargingTimeout) // Charging has reached Timeout period. Either battery will not hold charge, has high capacity or the TP5100 is faulty
			{
				digitalSwitch(module[i].chargeMosfetPin, 0); // Turn off TP5100
				module[i].batteryFaultCode = 9;				 // Set the Battery Fault Code to 7 Charging Timeout
				if (module[i].insertData == true)
				{
					clearSecondsTimer(i);
					module[i].insertData = false;
					module[i].cycleState = 7; // Charging Timeout. Battery is considered faulty set cycleState to Completed
					module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
				}
				sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
			}
			break;
		case 3: // Check Battery Milli Ohms
			module[i].cycleCount = module[i].cycleCount + milliOhms(i);
			module[i].tempMilliOhmsValue = module[i].tempMilliOhmsValue + module[i].milliOhmsValue;
			if (module[i].cycleCount == 4)
			{
				module[i].milliOhmsValue = module[i].tempMilliOhmsValue / 4;
				if (module[i].milliOhmsValue > settings.highMilliOhms) // Check if Milli Ohms is greater than the set high Milli Ohms value
				{
					module[i].batteryFaultCode = 3; // Set the Battery Fault Code to 3 High Milli Ohms
					module[i].cycleState = 7;		// Milli Ohms is high battery is considered faulty set cycleState to Completed
					module[i].cycleCount = 0;		// Reset cycleCount for use in other Cycles
				}
				else
				{
					if (module[i].minutes <= 1) // No need to rest the battery if it is already charged
					{
						module[i].cycleState = 5; // Check Battery Milli Ohms Completed set cycleState to Discharge Battery
						module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
					}
					else
					{
						module[i].cycleState = 4; // Check Battery Milli Ohms Completed set cycleState to Rest Battery
						module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
					}
				}
				clearSecondsTimer(i);
			}
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=3&MO%d=%d&CV%d=%d.%02d"), i, i, (int)module[i].milliOhmsValue, i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100);
			break;

		case 4:																 // Rest Battery
			module[i].batteryVoltage = readMux(module[i].batteryVolatgePin); // Get battery voltage
			module[i].batteryCurrentTemp = getTemperature(i);
			if (module[i].minutes == settings.restTimeMinutes) // Rest time
			{
				module[i].batteryInitialVoltage = module[i].batteryVoltage; // Reset Initial voltage
				clearSecondsTimer(i);
				module[i].cycleState = 5; // Rest Battery Completed set cycleState to Discharge Battery
			}
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=4&TI%d=%d&CT%d=%d&CV%d=%d.%02d"), i, i, (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)), i, module[i].batteryCurrentTemp, i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100);
			break;
		case 5: // Discharge Battery
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=5&TI%d=%d&IT%d=%d&IV%d=%d.%02d&CT%d=%d&CV%d=%d.%02d&HT%d=%d&MA%d=%d&DA%d=%d.%02d&MO%d=%d"), i, i, (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)), i, module[i].batteryInitialTemp, i, (int)module[i].batteryInitialVoltage, (int)(module[i].batteryInitialVoltage * 100) % 100, i, module[i].batteryCurrentTemp, i, (int)module[i].dischargeVoltage, (int)(module[i].dischargeVoltage * 100) % 100, i, module[i].batteryHighestTemp, i, (int)module[i].dischargeMilliamps, i, (int)module[i].dischargeAmps, (int)(module[i].dischargeAmps * 100) % 100, i, (int)module[i].milliOhmsValue);
			if (processTemperature(i) == 2)
			{
				//Battery Temperature is >= MAX Threshold considered faulty
				digitalSwitch(module[i].dischargeMosfetPin, 0); // Turn off Discharge Mosfet
				module[i].batteryFaultCode = 7;					// Set the Battery Fault Code to 7 High Temperature
				if (module[i].insertData == true)
				{
					clearSecondsTimer(i);
					module[i].insertData = false;
					module[i].cycleState = 7; // Temperature is high. Battery is considered faulty set cycleState to Completed
					module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
				}
				sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
			}
			else
			{
				if (dischargeCycle(i))
					module[i].cycleCount++;
				if (module[i].cycleCount >= 10)
				{
					digitalSwitch(module[i].dischargeMosfetPin, 0);			  // Turn off Discharge Mosfet
					if (module[i].dischargeMilliamps < settings.lowMilliamps) // No need to recharge the battery if it has low Milliamps
					{
						module[i].batteryFaultCode = 5; // Set the Battery Fault Code to 5 Low Milliamps
						if (module[i].insertData == true)
						{
							clearSecondsTimer(i);
							module[i].insertData = false;
							module[i].cycleState = 7; // Discharge Battery Completed set cycleState to Completed
							module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
						}
						sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
					}
					else
					{
						if (module[i].insertData == true)
						{
							module[i].batteryVoltage = readMux(module[i].batteryVolatgePin); // Get battery voltage for Recharge Cycle
							module[i].batteryInitialVoltage = module[i].batteryVoltage;		 // Reset Initial voltage
							clearSecondsTimer(i);
							module[i].insertData = false;
							module[i].cycleState = 6; // Discharge Battery Completed set cycleState to Recharge Battery
							module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
						}
						sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
					}
				}
			}
			break;
		case 6:																 // Recharge Battery
			module[i].batteryVoltage = readMux(module[i].batteryVolatgePin); // Get battery voltage
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=6&TI%d=%d&IT%d=%d&IV%d=%d.%02d&CT%d=%d&CV%d=%d.%02d&HT%d=%d"), i, i, (module[i].seconds + (module[i].minutes * 60) + (module[i].hours * 3600)), i, module[i].batteryInitialTemp, i, (int)module[i].batteryInitialVoltage, (int)(module[i].batteryInitialVoltage * 100) % 100, i, module[i].batteryCurrentTemp, i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100, i, module[i].batteryHighestTemp);
			if (processTemperature(i) == 2)
			{
				//Battery Temperature is >= MAX Threshold considered faulty
				digitalSwitch(module[i].chargeMosfetPin, 0); // Turn off TP5100
				module[i].batteryFaultCode = 7;				 // Set the Battery Fault Code to 7 High Temperature
				if (module[i].insertData == true)
				{
					clearSecondsTimer(i);
					module[i].insertData = false;
					module[i].cycleState = 7; // Temperature is to high. Battery is considered faulty set cycleState to Completed
					module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
				}
				sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
			}
			else
			{
				digitalSwitch(module[i].chargeMosfetPin, 1); // Turn on TP5100
				if (settings.storageChargeVoltage > 0.00)
				{
					if (module[i].batteryVoltage > (settings.storageChargeVoltage + 0.35))
						module[i].cycleCount++;
				}
				else
				{
					module[i].cycleCount = module[i].cycleCount + chargeCycle(i);
				}
				if (module[i].cycleCount >= 10)
				{
					digitalSwitch(module[i].chargeMosfetPin, 0); // Turn off TP5100
					if (module[i].insertData == true)
					{
						clearSecondsTimer(i);
						module[i].insertData = false;
						module[i].cycleState = 7; // Recharge Battery Completed set cycleState to Completed
						module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
					}
					sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
				}
			}
			if (module[i].hours == settings.chargingTimeout) // Charging has reached Timeout period. Either battery will not hold charge, has high capacity or the TP5100 is faulty
			{
				digitalSwitch(module[i].chargeMosfetPin, 0); // Turn off TP5100
				module[i].batteryFaultCode = 9;				 // Set the Battery Fault Code to 7 Charging Timeout
				if (module[i].insertData == true)
				{
					clearSecondsTimer(i);
					module[i].insertData = false;
					module[i].cycleState = 7; // Charging Timeout. Battery is considered faulty set cycleState to Completed
					module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
				}
				sprintf_P(serialSendString + strlen(serialSendString), PSTR("&ID%d"), i);
			}
			break;
		case 7: // Completed
			if (!batteryCheck(i))
				module[i].cycleCount++;
			if (module[i].cycleCount == 2)
			{
				module[i].cycleState = 0; // Completed and Battery Removed set cycleState to Check Battery Voltage
				module[i].cycleCount = 0; // Reset cycleCount for use in other Cycles
			}
			sprintf_P(serialSendString + strlen(serialSendString), PSTR("&CS%d=7&CV%d=%d.%02d&FC%d=%d"), i, i, (int)module[i].batteryVoltage, (int)(module[i].batteryVoltage * 100) % 100, i, module[i].batteryFaultCode);
			break;
		}
		secondsTimer(i);
	}
	cycleStateLCD();
	fanController();
}