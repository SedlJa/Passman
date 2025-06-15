/*
    Header file for rotary encoder functionality
*/
#ifndef ROTARY_H

#define ROTARY_H

/* Includes */
#include "AiEsp32RotaryEncoder.h"
#include "Arduino.h"

// Create an instance of the rotary encoder
extern AiEsp32RotaryEncoder rotaryEncoder;

/* Functions declarations */
void rotary_setup();             // Function to initialize the rotary encoder
void rotary_onButtonClick();     // Function to handle button click events
void rotary_loop();              // Function to handle rotary encoder loop events
void IRAM_ATTR readEncoderISR(); // Interrupt Service Routine for reading encoder values

#endif // ROTARY_H