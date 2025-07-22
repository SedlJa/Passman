/*
    Header file for fingerprint sensor check
*/

#ifndef FPCHECK_H

#define FPCHECK_H

/* Includes */
#include <Adafruit_Fingerprint.h>
#include "Arduino.h"

/* Functions declarations */
uint8_t getFingerprintID(); // Function which handles fingerprint input

int getFingerprintIDez(); // Function to get fingerprint ID from database

void fpSetup();        // Function to init fpSerial and check if sensor is found
void waitAfterMatch(); // Function for delay after succesfull match

#endif

/*
########################################################################
Disclaimer.

Some of these functions are customized public examples.
########################################################################
*/