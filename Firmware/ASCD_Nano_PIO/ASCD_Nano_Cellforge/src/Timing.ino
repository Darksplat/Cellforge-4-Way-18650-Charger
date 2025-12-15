
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
 * Timing and per-module variable initialisation.
 */

void secondsTimer(byte j)
{
	unsigned long longMilliSecondsCount = millis() - module[j].longMilliSecondsCleared;
	module[j].hours   = longMilliSecondsCount / (1000L * 60L * 60L);
	module[j].minutes = (longMilliSecondsCount % (1000L * 60L * 60L)) / (1000L * 60L);
	module[j].seconds = (longMilliSecondsCount % (1000L * 60L * 60L) % (1000L * 60L)) / 1000;
}

void clearSecondsTimer(byte j)
{
	module[j].longMilliSecondsCleared = millis();
	module[j].seconds = 0;
	module[j].minutes = 0;
	module[j].hours   = 0;
}

void initializeVariables(byte j)
{
	// Reset per-cycle values
	module[j].batteryBarcode        = false;
	module[j].insertData            = false;
	module[j].tempMilliOhmsValue    = 0;
	module[j].milliOhmsValue        = 0;
	module[j].intMilliSecondsCount  = 0;
	module[j].longMilliSecondsPreviousCount = 0;
	module[j].longMilliSecondsPrevious      = 0;
	module[j].longMilliSecondsPassed        = 0;
	module[j].dischargeMilliamps    = 0.0;
	module[j].dischargeVoltage      = 0.00;
	module[j].dischargeAmps         = 0.00;
	module[j].batteryFaultCode      = 0;
	module[j].batteryInitialTemp    = 0;
	module[j].batteryCurrentTemp    = 0;
	module[j].batteryHighestTemp    = 0;
}
