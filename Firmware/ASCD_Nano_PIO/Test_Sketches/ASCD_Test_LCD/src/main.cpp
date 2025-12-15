
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


#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Adjust address here if needed (common: 0x27 or 0x3F)
static constexpr uint8_t LCD_ADDRESS = 0x27;
static constexpr uint8_t LCD_COLS    = 16;
static constexpr uint8_t LCD_ROWS    = 2;

LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

void setup()
{
    Serial.begin(115200);
    delay(500);

    Serial.println();
    Serial.println("ASCD Nano - I2C LCD Screen Test");
    Serial.println("================================");
    Serial.println("This test checks:");
    Serial.println(" - I2C address ");
    Serial.println(" - Backlight");
    Serial.println(" - Both rows");
    Serial.println(" - All 16 columns per row");
    Serial.println();
    Serial.println("If you see nothing on the LCD:");
    Serial.println(" - Check wiring: SDA -> A4, SCL -> A5 on Nano");
    Serial.println(" - Check VCC (5V) and GND");
    Serial.println(" - Try changing LCD_ADDRESS (0x27 <-> 0x3F)");
    Serial.println();

    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Initial static message (similar to original)
    lcd.setCursor(0, 0);
    lcd.print("Testing LCD 16x2");
    lcd.setCursor(0, 1);
    lcd.print("0123456789ABCDEF");

    Serial.println("LCD initial text written:");
    Serial.println(" Row 0: \"Testing LCD 16x2\"");
    Serial.println(" Row 1: \"0123456789ABCDEF\"");
    Serial.println("If text is visible and correct, LCD is basically OK.");
    Serial.println();
}

void loop()
{
    // After initial text, cycle through some simple patterns to visually confirm all pixels

    // 1) Clear + solid block of hashes on row 0
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("################");
    lcd.setCursor(0, 1);
    lcd.print("Row0 all cols   ");
    Serial.println("Pattern: Row 0 filled with '#', Row 1 label.");
    delay(2000);

    // 2) Solid block on row 1
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Row1 all cols   ");
    lcd.setCursor(0, 1);
    lcd.print("################");
    Serial.println("Pattern: Row 1 filled with '#', Row 0 label.");
    delay(2000);

    // 3) Diagonal-ish pattern (even columns on row 0, odd on row 1)
    lcd.clear();
    for (uint8_t col = 0; col < LCD_COLS; col++) {
        lcd.setCursor(col, (col % 2 == 0) ? 0 : 1);
        lcd.print('*');
    }
    Serial.println("Pattern: '*' alternating between row 0 and row 1 across the screen.");
    delay(2000);

    // 4) Back to the original test text
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Testing LCD 16x2");
    lcd.setCursor(0, 1);
    lcd.print("0123456789ABCDEF");
    Serial.println("Pattern: Original test text restored.");
    delay(3000);
}
