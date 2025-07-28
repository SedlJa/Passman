/* Includes */
#include "fpcheck.h"

/* Defines */
#define RX_PIN GPIO_NUM_16 // GPIO PIN 16 as RX
#define TX_PIN GPIO_NUM_17 // GPIO PIN 17 as TX
#define FPBAUD 57600

HardwareSerial fpSerial(2); // Use Serial2 for communication
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fpSerial);

/* Functions */

/*
    @brief Sets up the fingerprint sensor and starts fpSerial communication
*/
void fpSetup()
{
    fpSerial.begin(FPBAUD, SERIAL_8N1, TX_PIN, RX_PIN); // Start fpSerial
    delay(100);

    /* Check if sensor is found */
    finger.begin(FPBAUD);
    delay(5);
    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!\r\n");
    }
    else
    {
        Serial.println("\r\nDid not find fingerprint sensor :(");
        while (1)
        {
            delay(1);
        }
    }

    /* Check if fp sensor contains any finger template */
    finger.getTemplateCount();
    if (finger.templateCount == 0)
    {
        Serial.print("Sensor doesn't contain any fingerprint data. Please run the 'enroll' utility.");
    }
    else
    {
        Serial.println("Waiting for valid finger...");
        Serial.print("Sensor contains ");
        Serial.print(finger.templateCount);
        Serial.println(" templates");
    }
}

/*
    @brief FP ID handler
    @return fingerID – ID of matched fingerprint
*/
uint8_t getFingerprintID()
{
    uint8_t p = finger.getImage();
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
    case FINGERPRINT_NOFINGER:
        Serial.println("No finger detected");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK success!

    p = finger.image2Tz();
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Found a print match!");
        return 1;
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_NOTFOUND)
    {
        Serial.println("Did not find a match");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    delay(1000);

    // return finger.fingerID;
    return 1;
}

/*
    @brief Function that handles device unlock procedure
    @param handler  handler for global use
*/
void fpUnlockDevice(int handler)
{
    /* Unlock procedure */
    while (1)
    {
        unlockPage(); // Show unlock page

        handler = getFingerprintID(); // Enables scanning and saves return value

        if (handler == 1)
        {
            Serial.println((String) "Device UNLOCKED, unlockHandler value: " + (String)handler);
            break;
        }
    }

    // Show Tick when succesfully logged in
    unlockSuccessfull();
}

/*
    @brief Function that gets FP ID and counts a confidence factor
    @return -1 fail
            ; otherwise ID #
*/
int getFingerprintIDez()
{
    uint8_t p = finger.getImage();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
        return -1;

    // found a match!
    Serial.print("Found ID #");
    Serial.print(finger.fingerID);
    Serial.print(" with confidence of ");
    Serial.println(finger.confidence);
    return finger.fingerID;
}