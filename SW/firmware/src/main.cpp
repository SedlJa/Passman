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

  String plainText = "Hello from ESP32";
  // Encrypt the data
  std::vector<uint8_t> encrypted_payload = encrypt_data(plainText.c_str());

  if (!encrypted_payload.empty())
  {
    // 1. Convert the final IV + Ciphertext payload to Base64 for clean transmission
    size_t output_len;
    mbedtls_base64_encode(NULL, 0, &output_len, encrypted_payload.data(), encrypted_payload.size());
    std::vector<unsigned char> base64_buf(output_len);
    mbedtls_base64_encode(base64_buf.data(), base64_buf.size(), &output_len, encrypted_payload.data(), encrypted_payload.size());

    String base64_output = (char *)base64_buf.data();

    Serial.printf("Original Plaintext: %s\n", plainText.c_str());
    Serial.printf("Encrypted (Base64 for TX): %s\n", base64_output.c_str());
    // Send 'base64_output' over your communication channel (Wi-Fi, Serial, etc.)
  }
  else
  {
    Serial.println("Encryption failed!");
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
                  Serial.println("id;username;password");
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
