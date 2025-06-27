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
void page1(int rotaryVal);
void page2(int rotaryVal);

#endif // OLED_H