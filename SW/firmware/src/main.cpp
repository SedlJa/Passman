#include <Arduino.h>
#include "rotary.h"
#include "oled.h"
#include "fpcheck.h"
#include "database.h"

/* Global variables */
int unlockHandler = 0;          // Handles authentication
bool connectionHandler = false; // Handles connection to webUI
bool dbHandler = true;
bool stringComplete = false;
String receivedData = " ";

void setup()
{
  Serial.begin(9600);

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

  // Reserve memory for the String to reduce memory fragmentation
  receivedData.reserve(200);
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
      connectingPage3();
      // readSerialData(receivedData, stringComplete);
      Serial.println("load");
      while (true)
      {
        if (Serial.available() > 0)
        {
          String message = Serial.readStringUntil('\n');
          message.trim(); // Remove any trailing whitespace or newline characters
          if (message == "ok")
          {
            break;
          }
        }
      }
      Serial.println("1;username;password");
      Serial.println("2;username;password");
      Serial.println("3;username;password");
      Serial.println("4;username;password");
      Serial.println("end");
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
