#include <Arduino.h>
#include "rotary.h"
#include "oled.h"
#include "fpcheck.h"
#include "database.h"
#include "encryption.h"
#include "authentication.h"

#define BAUDRATE 115200
#define USERLED 13 // blue user led definition

/* HELLO */

/* Global variables */
int unlockHandler = 0; // Handles authentication
int fails = 0;
int uploadHandler = 0;     // Handles upload action
int connectionHandler = 0; // Handles connection to the app
int downloadHandler = 0;   // Handles download action
int dbLength = 0;          // Updates database legth

void setup()
{
  Serial.begin(BAUDRATE);
  /* User LED setup */
  pinMode(USERLED, OUTPUT);

  /* Battery level setup */
  pinMode(3, INPUT);

  /* Rotary encoder init */
  rotary_setup();

  /* OLED display init */
  display_setup();

  /* Finger Print sensor init */
  fpSetup(); // Sets up fp sensor

  /* Unlock Procedure */
  fpUnlockDevice(unlockHandler, USERLED);
  /* Unlock via pin */
  insertPin();

  /* Init SPIFFS */
  init_spiffs_db();

  /* update db length */
  dbLength = updateDbLength();
}

void loop()
{
  // BASIC MENU
  if (rotaryEncoder.readEncoder() == 1) // Database
  {
    menuPage1();
    if (rotaryEncoder.isEncoderButtonClicked(300))
    {
      rotaryEncoder.setBoundaries(0, dbLength - 1, false); // update boundaries to db lenght
      rotaryEncoder.setEncoderValue(0);
      while (!rotaryEncoder.isEncoderButtonClicked())
      {
        databasePage(rotaryEncoder.readEncoder()); // show database
      }
      rotaryEncoder.setBoundaries(1, 6, false); // update boundaries to db lenght
    }
  }

  // Connect to APP
  else if (rotaryEncoder.readEncoder() == 2 && 3) // Upload
  {
    menuPage2();

    if (rotaryEncoder.isEncoderButtonClicked(300)) // When encoder is clicked
    {
      // Open the JSON file
      FILE *file = fopen("/spiffs/device.json", "r");
      if (!file)
      {
        Serial.println("Failed to open device.json");
        return;
      }

      // Read the file into a buffer
      char buffer[256];
      size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, file);
      buffer[bytesRead] = '\0'; // Null-terminate the buffer
      fclose(file);

      // Parse the JSON data
      StaticJsonDocument<512> doc;
      DeserializationError error = deserializeJson(doc, buffer);
      if (error)
      {
        Serial.print("Failed to parse JSON: ");
        Serial.println(error.c_str());
        return;
      }

      // Extract device information
      JsonObject deviceInfo = doc["deviceInfo"];
      const char *deviceName = deviceInfo["device Name"];
      const char *fwVersion = deviceInfo["FW Version"];
      const char *hwVersion = deviceInfo["HW Version"];
      const char *serialNumber = deviceInfo["SN"];

      // Send device information to the app
      Serial.println("Device Information:");
      Serial.println((String) "Name: " + deviceName);
      Serial.println((String) "FW: " + fwVersion);
      Serial.println((String) "HW: " + hwVersion);
      Serial.println((String) "SN: " + serialNumber);

      // Turn on USER LED to indicate connection
      connectionHandler = 1;
      digitalWrite(USERLED, HIGH);
      //  Serial.println("Command: upload");
      while (connectionHandler == 1)
      {
        digitalWrite(USERLED, HIGH);
        // load command
        if (Serial.available() > 0)
        {
          String message = Serial.readStringUntil('\n');
          message.trim(); // Remove any trailing whitespace or newline characters
          if (message == "load")
          {
            Serial.println("data");
            for (int i = 0; i < dbLength; i++)
            {
              Serial.printf("%s;%s;%s\n", encrypt_data(db.id[i].c_str()).c_str(), encrypt_data(db.username[i].c_str()).c_str(), encrypt_data(db.password[i].c_str()).c_str());
            }
          }
          // Download command
          else if (message == "download")
          {
            // Read the database length from the next line
            while (Serial.available() == 0)
            {
              // Wait for data
            }
            String lengthLine = Serial.readStringUntil('\n');
            lengthLine.trim();
            int newDbLength = lengthLine.toInt();
            dbLength = newDbLength; // update db length

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
          }
          // Handle info command
          else if (message == "info")
          {
            // Send device information to the app
            Serial.println("Device Information:");
            Serial.println((String) "Name: " + deviceName);
            Serial.println((String) "FW: " + fwVersion);
            Serial.println((String) "HW: " + hwVersion);
            Serial.println((String) "SN: " + serialNumber);
          }
          // Handle "disconnect" command
          else if (message == "disconnect")
          {
            connectionHandler = 0;
            Serial.println("Disconnected from app.");
            digitalWrite(USERLED, LOW); // Turn off USER LED
            break;
          }
        }
      }
      //}
    }
  }

  // DOWNLOAD DATABASE
  /*
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
  */
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
        deviceInfo();
      }
    }
  }
}
