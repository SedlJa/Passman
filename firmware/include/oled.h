/*
    Header file for OLED display
*/

#ifndef OLED_H

#define OLED_H

/* Includes */
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>

// Create instance of OLED display
extern Adafruit_SH1106G display;

/* Functions */
void display_setup();
void test_print(int rotaryVal);

/* Main Menu pages*/
void menuPage1(int rotaryVal);
void menuPage2(int rotaryVal);
void menuPage3(int rotaryVal);

/* Author page */
void authorInfo();

#endif // OLED_H