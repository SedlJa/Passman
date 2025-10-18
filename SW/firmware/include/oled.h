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
extern const unsigned char epd_bitmap_connecting1[];
extern const unsigned char epd_bitmap_connecting2[];
extern const unsigned char epd_bitmap_connecting3[];
extern const unsigned char epd_bitmap_fpsensor[];
extern const unsigned char epd_bitmap_tick[];
extern const unsigned char epd_bitmap_tick2[];
extern const unsigned char epd_bitmap_tick3[];
extern const unsigned char epd_bitmap_connected_to_app[];
extern const unsigned char epd_bitmap_disconnected_from_app[];

/* Functions */
void display_setup();
void test_print(int rotaryVal);

/* Unlock page */
void unlockPage();        // while scanning fingerprint
void unlockSuccessfull(); // Shows tick while authentication passed

/* Main Menu pages */
void menuPage1(); // database
void menuPage2(); // connect
void menuPage3(); // settings

/* Connecting page */
void connectingPage1();
void connectingPage2();
void connectingPage3();
void connectedToApp();
void disconnectedFromApp();

/* Database */
void databasePage(int dbEntry);

/* Author page */
void authorInfo();

#endif // OLED_H