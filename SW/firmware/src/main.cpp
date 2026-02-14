#include <Arduino.h>
#include "rotary.h"
#include "oled.h"
#include "fpcheck.h"
#include "database.h"
#include "encryption.h"

#define BAUDRATE 115200
#define USERLED 13 // blue user led definition

/* HELLO */

/* Global variables */
int unlockHandler = 0;          // Handles authentication
int uploadHandler = 0;          // Handles upload action
int downloadHandler = 0;        // Handles download action
bool connectionHandler = false; // Handles connection to webUI
bool dbHandler = true;
bool stringComplete = false;
String receivedData = " ";

void setup()
{
  Serial.begin(BAUDRATE);
  /* User LED setup */
  pinMode(USERLED, OUTPUT);
  /* Rotary encoder init */
  rotary_setup();

  /* OLED display init */
  display_setup();

  /* Finger Print sensor init */
  fpSetup(); // Sets up fp sensor

  /* Unlock Procedure - fingerprint */
  fpUnlockDevice(unlockHandler, USERLED);

  /* Init SPIFFS */
  init_spiffs_db();
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
      rotaryEncoder.setBoundaries(1, 6, false); // update boundaries to db lenght
    }
  }

  // UPLOAD DATABASE
  else if (rotaryEncoder.readEncoder() == 2) // Upload
  {
    menuPage2();

    if (rotaryEncoder.isEncoderButtonClicked())
    {
      uploadHandler = 1;
    }
    //  Serial.println("Command: upload");
    while (uploadHandler == 1)
    {
      if (Serial.available() > 0)
      {
        String message = Serial.readStringUntil('\n');
        message.trim(); // Remove any trailing whitespace or newline characters
        if (message == "load")
        {
          Serial.println("data");
          for (int i = 0; i < DB_LENGTH; i++)
          {
            Serial.printf("%s;%s;%s\n", encrypt_data(db.id[i].c_str()).c_str(), encrypt_data(db.username[i].c_str()).c_str(), encrypt_data(db.password[i].c_str()).c_str());
          }
          uploadHandler = 0;
          break;
        }
      }
    }
    //}
  }

  // DOWNLOAD DATABASE
  else if (rotaryEncoder.readEncoder() == 3) // Download
  {
    menuPage3();
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      downloadHandler = 1;
    }
    while (downloadHandler == 1)
    {
      if (Serial.available() > 0)
      {
        String message = Serial.readStringUntil('\n');
        message.trim(); // Remove any trailing whitespace or newline characters
        if (message == "download")
        {
          // Read the database length from the next line
          while (Serial.available() == 0)
          {
            // Wait for data
          }
          String lengthLine = Serial.readStringUntil('\n');
          lengthLine.trim();
          int newDbLength = lengthLine.toInt();

          for (int i = 0; i < newDbLength; i++)
          {
            while (Serial.available() == 0)
            {
              // Wait for data
            }
            String dataLine = Serial.readStringUntil('\n');
            dataLine.trim();                // Remove any trailing whitespace or newline characters
            parseAndStoreData(dataLine, i); // data are being decrypted in this function
            dataLine = "";
          }
          downloadHandler = 0;
          break;
        }
      }
    }
  }

  // SAVE DATABASE
  else if (rotaryEncoder.readEncoder() == 4) // Save
  {
    menuPage4(); // You may want a dedicated menu page for Save
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      save_spiffs_db();
    }
  }

  // DELETE ALL DATABASE
  else if (rotaryEncoder.readEncoder() == 5) // Delete All
  {
    menuPage5(); // You may want a dedicated menu page for Delete All
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      delete_spiffs_db();
    }
  }
  // SHOW DEVICE INFORMATION
  else if (rotaryEncoder.readEncoder() == 6) // Settings
  {
    menuPage6();
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      while (!rotaryEncoder.isEncoderButtonClicked())
      {
        authorInfo();
      }
    }
  }
}
