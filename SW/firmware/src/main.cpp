#include <Arduino.h>
#include "rotary.h"
#include "oled.h"
#include "fpcheck.h"
#include "database.h"
#include "encryption.h"

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

  String plainTextID = db.id[0];
  String plainTextUsername = db.username[0];
  String plainTextPassword = db.password[0];
  // Encrypt the data
  Serial.printf("Encrypted (Base64 for TX): %s\n", encrypt_data(plainTextUsername.c_str()).c_str());
}

void loop()
{

  // BASIC MENU
  if (rotaryEncoder.readEncoder() == 1) // Database
  {
    menuPage1();
    if (rotaryEncoder.isEncoderButtonClicked(300))
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
      rotaryEncoder.setBoundaries(1, 2, true); // Update boundaries
      rotaryEncoder.setEncoderValue(1);        // set value to 1
      // bool choose = false;
      while (!rotaryEncoder.isEncoderButtonDown())
      {
        if (rotaryEncoder.readEncoder() == 1) // Upload database to app
        {
          uploadPage();
          if (rotaryEncoder.isEncoderButtonClicked())
          {
            // Serial.println("Command: upload");
            while (true)
            {
              if (Serial.available() > 0)
              {
                String message = Serial.readStringUntil('\n');
                message.trim(); // Remove any trailing whitespace or newline characters
                if (message == "load")
                {
                  Serial.println("data");
                  Serial.printf("%s;%s;%s\n", encrypt_data(db.id[0].c_str()).c_str(), encrypt_data(db.username[0].c_str()).c_str(), encrypt_data(db.password[0].c_str()).c_str());
                  break;
                }
              }
            }
          }
        }
        else if (rotaryEncoder.readEncoder() == 2) // Download database from app
        {
          downloadPage();
          if (rotaryEncoder.isEncoderButtonClicked())
          {
            while (true)
            {
              if (Serial.available() > 0)
              {
                String message = Serial.readStringUntil('\n');
                message.trim(); // Remove any trailing whitespace or newline characters
                if (message == "download")
                {
                  for (int i = 0; i < 4; i++)
                  {
                    while (Serial.available() == 0)
                    {
                      // Wait for data
                    }
                    String dataLine = Serial.readStringUntil('\n');
                    dataLine.trim(); // Remove any trailing whitespace or newline characters
                    parseAndStoreData(dataLine, i);
                    dataLine = "";
                  }
                  break;
                }
              }
            }
          }
        }
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
