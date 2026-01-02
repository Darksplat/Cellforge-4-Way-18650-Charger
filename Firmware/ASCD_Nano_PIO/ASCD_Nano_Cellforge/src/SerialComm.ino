/*
// ASDC Nano 4x Arduino Charger / Discharger
// ---------------------------------------------------------------------------
// Created by Brett Watt on 19/03/2019
// Modified by Jeremy Younger @darksplat on 06/12/2025
*/

#include <Arduino.h>

// =====================================================
// Serial communication handler
// =====================================================

void sendSerial()
{
  if (strlen(serialSendString) > 0)
  {
    Serial.println(serialSendString);
    strcpy(serialSendString, "");
    readSerialResponse = false;
  }
}



void readSerial()
{
  // Host PC handles barcode + database.
  readSerialResponse = false;
}



// =====================================================
// Parse colon-separated return codes
// =====================================================

void parseReturnCodes(const char *msg)
{
  char temp[128];
  strncpy(temp, msg, sizeof(temp));
  temp[sizeof(temp) - 1] = '\0';

  char *token = strtok(temp, ":");
  while (token != nullptr)
  {
    int code = atoi(token);
    returnCodes(code);
    token = strtok(nullptr, ":");
  }
}


// =====================================================
// Handle return codes
// =====================================================

void returnCodes(int codeID)
{
  switch (codeID)
  {
    case 0: Serial.println(F("SUCCESSFUL")); break;
    case 1: Serial.println(F("CONNECTION_ERROR")); break;
    case 2: Serial.println(F("TIMEOUT")); break;
    case 3: Serial.println(F("ERROR_DATABASE")); break;
    case 4: Serial.println(F("ERROR_MISSING_DATA")); break;
    case 5: Serial.println(F("ERROR_NO_BARCODE_DB")); break;
    case 6: Serial.println(F("ERROR_NO_BARCODE_INPUT")); break;
    case 7: Serial.println(F("ERROR_DATABASE_HASH_INPUT")); break;
    case 8: Serial.println(F("ERROR_HASH_INPUT")); break;
    case 9: Serial.println(F("ERROR_SERIAL_OUTPUT")); break;

    // Barcode continue
    case 100: module[0].batteryBarcode = true; break;
    case 101: module[1].batteryBarcode = true; break;
    case 102: module[2].batteryBarcode = true; break;
    case 103: module[3].batteryBarcode = true; break;

    // Insert data acknowledged
    case 200: module[0].insertData = true; break;
    case 201: module[1].insertData = true; break;
    case 202: module[2].insertData = true; break;
    case 203: module[3].insertData = true; break;

    default:
      Serial.println(F("UNKNOWN"));
      break;
  }
}
