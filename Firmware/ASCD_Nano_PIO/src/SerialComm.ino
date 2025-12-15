
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
 * Serial communication to ESP8266 and USB serial.
 * Handles sending status packets and processing return codes.
 */

void sendSerial()
{
	if (strcmp(serialSendString, "") != 0)
	{
		ESP8266.println(serialSendString);
		Serial.println(serialSendString);
		strcpy(serialSendString, "");
		readSerialResponse = true;
	}
}

void readSerial()
{
	while (ESP8266.available())
	{
		String returnString = "";
		String recievedMessage = "";
		int returnInt;

		recievedMessage = ESP8266.readString();
		Serial.println(recievedMessage);
		recievedMessage.trim();

		if (recievedMessage.length() > 1) // Multiple chars
		{
			for (byte i = 0; i <= recievedMessage.length(); i++)
			{
				if (recievedMessage.charAt(i) == ':' || recievedMessage.length() == i)
				{
					returnInt = returnString.toInt();
					if (returnInt != 0)
					{
						returnCodes(returnInt);
					}
					else
					{
						returnCodes(9); // ERROR_SERIAL_OUTPUT
					}
					returnString = "";
				}
				else
				{
					returnString += recievedMessage.charAt(i);
				}
			}
		}
		else
		{
			// Single character / code
			returnInt = recievedMessage.toInt();
			returnCodes(returnInt);
		}
		countSerialSend = 0;
		readSerialResponse = false;
	}
}

void returnCodes(int codeID)
{
	switch (codeID)
	{
	case 0:
		Serial.println(F("SUCCESSFUL"));
		break;
	case 1:
		Serial.println(F("CONNECTION_ERROR"));
		break;
	case 2:
		Serial.println(F("TIMEOUT"));
		break;
	case 3:
		Serial.println(F("ERROR_DATABASE"));
		break;
	case 4:
		Serial.println(F("ERROR_MISSING_DATA"));
		break;
	case 5:
		Serial.println(F("ERROR_NO_BARCODE_DB"));
		break;
	case 6:
		Serial.println(F("ERROR_NO_BARCODE_INPUT"));
		break;
	case 7:
		Serial.println(F("ERROR_DATABASE_HASH_INPUT"));
		break;
	case 8:
		Serial.println(F("ERROR_HASH_INPUT"));
		break;
	case 9:
		Serial.println(F("ERROR_SERIAL_OUTPUT"));
		break;

	// Barcode continue – mark module as having a valid barcode
	case 100:
		module[0].batteryBarcode = true;
		Serial.println(F("BARCODE_CONTINUE_0"));
		break;
	case 101:
		module[1].batteryBarcode = true;
		Serial.println(F("BARCODE_CONTINUE_1"));
		break;
	case 102:
		module[2].batteryBarcode = true;
		Serial.println(F("BARCODE_CONTINUE_2"));
		break;
	case 103:
		module[3].batteryBarcode = true;
		Serial.println(F("BARCODE_CONTINUE_3"));
		break;

	// Insert data successful – server acknowledged cycle data
	case 200:
		module[0].insertData = true;
		Serial.println(F("INSERT_DATA_SUCCESSFUL_0"));
		break;
	case 201:
		module[1].insertData = true;
		Serial.println(F("INSERT_DATA_SUCCESSFUL_1"));
		break;
	case 202:
		module[2].insertData = true;
		Serial.println(F("INSERT_DATA_SUCCESSFUL_2"));
		break;
	case 203:
		module[3].insertData = true;
		Serial.println(F("INSERT_DATA_SUCCESSFUL_3"));
		break;

	default:
		Serial.println(F("UKNOWN"));
		break;
	}
}
