#include <Arduino.h>
#include "rotary.h"
#include "oled.h"

/* Global variables */
bool connectionHandler = false; // Handles connection to webUI

void setup()
{
  Serial.begin(115200);

  /* Rotary encoder init */
  rotary_setup();

  /* OLED display init */
  display_setup();
}

void loop()
{
  // BASIC MENU
  if (rotaryEncoder.readEncoder() == 1) // Database
  {
    menuPage1();
  }
  else if (rotaryEncoder.readEncoder() == 2) // Connect
  {
    menuPage2();
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      while (connectionHandler != true)
      {
        // Showing connection progress while connecting to webUI
        Serial.println((String) "Connecting to webUI..."); // debug msg

        connectingPage1();
        delay(500);
        connectingPage2();
        delay(500);
        connectingPage3();
        delay(500);
      }
    }
  }
  else if (rotaryEncoder.readEncoder() == 3) // Settings
  {
    menuPage3();

    if (rotaryEncoder.isEncoderButtonClicked())
    {
      while (!rotaryEncoder.isEncoderButtonClicked())
      {
        authorInfo();
      }
    }
  }
  else
  {
    test_print(rotaryEncoder.readEncoder());
  }
}
