/*
    Header file for Authentication
*/

#ifndef AUTHENTICATION_H

#define AUTHENTICATION_H

/* Includes */
#include <Arduino.h>
#include "rotary.h"
#include "oled.h"

/* PIN page */
int comparePins(int setPin[], int insertedPin[]);
void insertPin();

#endif // AUTHENTICATION_H