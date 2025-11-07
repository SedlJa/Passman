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

  // This is the Base64 string from the Python encryption example: "ACK_OK"
  const String encrypted_from_python = "Lc9GKQS0eNhop/8/K/H82Yl8KAzYKjNGMz27u9y8b+o=";
  Serial.print("Encrypted (Base64 from Python): ");
  Serial.println(encrypted_from_python);

  String decrypted_message = decrypt_data(encrypted_from_python);

  if (decrypted_message.length() > 0)
  {
    Serial.print("\n**Decrypted (ESP32 String): ");
    Serial.print(decrypted_message);
    Serial.println("**");
  }
  else
  {
    Serial.println("\nDecryption failed.");
  }
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
                  for (int i = 0; i < DB_LENGTH; i++)
                  {
                    Serial.printf("%s;%s;%s\n", encrypt_data(db.id[i].c_str()).c_str(), encrypt_data(db.username[i].c_str()).c_str(), encrypt_data(db.password[i].c_str()).c_str());
                  }
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
                  for (int i = 0; i < 6; i++)
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
