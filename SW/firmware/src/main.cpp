#include <Arduino.h>
#include "rotary.h"
#include "oled.h"
#include "fpcheck.h"
#include "database.h"

/* Global variables */
int unlockHandler = 0;          // Handles authentication
bool connectionHandler = false; // Handles connection to webUI
bool dbHandler = true;

void setup()
{
  Serial.begin(115200);

  /* Rotary encoder init */
  rotary_setup();

  /* OLED display init */
  display_setup();

  /* Finger Print sensor init */
  fpSetup(); // Sets up fp sensor

  /* Unlock Procedure - fingerprint */
  fpUnlockDevice(unlockHandler);

  /* database init */
  loadDatabase();
}

void loop()
{

  // BASIC MENU
  if (rotaryEncoder.readEncoder() == 1) // Database
  {
    menuPage1();
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      rotaryEncoder.setBoundaries(0, DB_LENGTH - 1, false); // update boundaries to db lenght
      rotaryEncoder.setEncoderValue(0);
      while (!rotaryEncoder.isEncoderButtonClicked())
      {
        databasePage(rotaryEncoder.readEncoder()); // show database
      }
      rotaryEncoder.setBoundaries(1, 3, false); // update boundaries to db lenght
    }
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
}
