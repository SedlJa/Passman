/*
    Header file for OLED display
*/

#ifndef OLED_H

#define OLED_H

/* Includes */
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "esp_spiffs.h"

// Create instance of OLED display
extern Adafruit_SH1106G display;

/* Instances for main menu images */
extern const unsigned char epd_bitmap_database[];
extern const unsigned char epd_bitmap_fpsensor[];
extern const unsigned char epd_bitmap_tick[];
extern const unsigned char epd_bitmap_tick2[];
extern const unsigned char epd_bitmap_tick3[];
extern const unsigned char epd_bitmap_download[];
extern const unsigned char epd_bitmap_upload[];
extern const unsigned char epd_bitmap_savedb[];
extern const unsigned char epd_bitmap_deleteall[];
extern const unsigned char epd_bitmap_devinfo[];
extern const unsigned char epd_bitmap_connect[];

/* Functions */
int getBatteryLevelPercentage(int pin);
void display_setup();
void test_print(int rotaryVal);

/* Unlock page */
void unlockPage();                  // while scanning fingerprint
void unlockSuccessfull(int ledpin); // Shows tick while authentication passed

/* Main Menu pages */
void menuPage1(); // database
void menuPage2(); // upload
void menuPage3(); // download
void menuPage4(); // save
void menuPage5(); // delete all
void menuPage6(); // dev info

/* Connecting page */
void connectingPage1();
void connectingPage2();
void connectingPage3();
void connectedToApp();
void disconnectedFromApp();
void uploadPage();
void downloadPage();

/* Database */
void databasePage(int dbEntry);

/* Author page */
void deviceInfo();

#endif // OLED_H