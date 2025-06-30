#include "authentication.h"

#define PINSIZE 4

/* Pin instance */
int pin[4] = {1, 9, 2, 9};

int comparePins(int setPin[], int insertedPin[])
{
    int ok = 1; // help variable to resolve inf inserted pin is correct

    for (int i = 0; i < PINSIZE; i++) // handles if each of inserted pin digits match
    {
        if (!(setPin[i] == insertedPin[i]))
        {
            ok = 0;
            Serial.println("Digit " + (String)i + " is incorrect!");
            delay(250);
        }
        else
        {
            Serial.println("Digit " + (String)i + " is correct!");
            delay(250);
        }
    }

    if (ok == 1)
    {
        Serial.println("Pin inserted successfully...");
        return 1;
    }
    else
    {
        Serial.println("Pin does not match....");
        return 0;
    }
}

/*
    @brief This function is the first step authentication, insert 4-digit pin
*/
void insertPin()
{
    uint8_t currPos;
    uint8_t unlock = 0; // 0 device locked, 1 device unlocked
    int insertedPin[4];

    // set encoder boundaries to <0;9>
    rotaryEncoder.setBoundaries(0, 9, true);
    currPos = 0;

    while (unlock == 0)
    {
        switch (currPos)
        {
        case 0:
            /* Handle 1st digit */
            display.setCursor(25, 25);                            // set cursor position
            display.println((String)rotaryEncoder.readEncoder()); // display current value of the encoder
            display.setCursor(25, 32);
            display.println((String) "-");
            display.display();
            delay(10);
            display.clearDisplay();

            if (rotaryEncoder.isEncoderButtonClicked())
            {
                insertedPin[currPos] = rotaryEncoder.readEncoder(); // assign curr value of the encoder to a pin variable
                currPos++;                                          // interate current position
                rotaryEncoder.setEncoderValue(0);                   // reset encoder value
            }
            break;

        case 1:
            /* Handle 2nd digit */
            display.setCursor(25, 25);                                                            // set cursor position
            display.println((String)insertedPin[0] + "  " + (String)rotaryEncoder.readEncoder()); // display current value of the encoder
            display.setCursor(43, 32);
            display.println((String) "-");
            display.display();
            delay(10);
            display.clearDisplay();

            if (rotaryEncoder.isEncoderButtonClicked())
            {
                insertedPin[currPos] = rotaryEncoder.readEncoder(); // assign curr value of the encoder to a pin variable
                currPos++;                                          // interate current position
                rotaryEncoder.setEncoderValue(0);                   // reset encoder value
            }
            break;

        case 2:
            /* Handle 3rd digit */
            display.setCursor(25, 25);                                                                                            // set cursor position
            display.println((String)insertedPin[0] + "  " + (String)insertedPin[1] + "  " + (String)rotaryEncoder.readEncoder()); // display current value of the encoder
            display.setCursor(61, 32);
            display.println((String) "-");
            display.display();
            delay(10);
            display.clearDisplay();

            if (rotaryEncoder.isEncoderButtonClicked())
            {
                insertedPin[currPos] = rotaryEncoder.readEncoder(); // assign curr value of the encoder to a pin variable
                currPos++;                                          // interate current position
                rotaryEncoder.setEncoderValue(0);                   // reset encoder value
            }
            break;

        case 3:
            /* Handle 4th digit */
            display.setCursor(25, 25);                                                                                                                            // set cursor position
            display.println((String)insertedPin[0] + "  " + (String)insertedPin[1] + "  " + (String)insertedPin[2] + "  " + (String)rotaryEncoder.readEncoder()); // display current value of the encoder
            display.setCursor(79, 32);
            display.println((String) "-");
            display.display();
            delay(10);
            display.clearDisplay();

            if (rotaryEncoder.isEncoderButtonClicked())
            {
                insertedPin[currPos] = rotaryEncoder.readEncoder(); // assign curr value of the encoder to a pin variable
                currPos++;                                          // interate current position
                rotaryEncoder.setEncoderValue(0);                   // reset encoder value
            }
            break;

        case 4:
            /* Handles if Pins match */
            unlock = comparePins(pin, insertedPin); // if unlock = 1, while cycle should break
            if (unlock == 0)
            {
                currPos = 0;
                Serial.println("Insert pin again...");
            }
            currPos = 5;
            break;

        default:

            break;
        }
    }
}
