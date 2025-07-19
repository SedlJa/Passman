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

/* Instances for main menu images */
extern const unsigned char epd_bitmap_database[];
extern const unsigned char epd_bitmap_connect[];
extern const unsigned char epd_bitmap_settings[];

/* Functions */
void display_setup();
void test_print(int rotaryVal);

/* Main Menu pages*/
void menuPage1(); // database
void menuPage2(); // connect
void menuPage3(); // settings

/* Author page */
void authorInfo();

#endif // OLED_H