#include "rotary.h"

/* Rotary PINS */
#define ROTARY_ENCODER_A_PIN 10
#define ROTARY_ENCODER_B_PIN 11
#define ROTARY_ENCODER_BUTTON_PIN 9
#define ROTARY_ENCODER_VCC_PIN -1
#define ROTARY_ENCODER_STEPS 4

// Create an instance of the rotary encoder
AiEsp32RotaryEncoder rotaryEncoder = AiEsp32RotaryEncoder(ROTARY_ENCODER_A_PIN, ROTARY_ENCODER_B_PIN, ROTARY_ENCODER_BUTTON_PIN, ROTARY_ENCODER_VCC_PIN, ROTARY_ENCODER_STEPS);

/*
    @brief function that setsup rotary encoder
*/
void rotary_setup()
{
    uint8_t initVal = 0; // Initialize the rotary encoder value

    rotaryEncoder.begin();               // Initialize rotary encoder
    rotaryEncoder.setup(readEncoderISR); // Setup the encoder with the ISR

    // Set boundaries and if values should cycle or not
    bool circleValues = false;
    rotaryEncoder.setBoundaries(1, 3, circleValues); // minValue, maxValue, circleValues true|false (when max go to min and vice versa)
    rotaryEncoder.setAcceleration(1);                // Set acceleration factor

    // Check if encoder is initialized
    if (initVal == 0)
    {
        Serial.println("Rotary encoder initialized...");
        delay(2000); // Init delay
    }
    else
    {
        Serial.println("Rotary encoder not initialized...");
        delay(2000);
    }
}

/*
    @brief function for handling encoder button click and printing into serial
*/
void rotary_onButtonClick()
{
    static unsigned long lastTimePressed = 0; // Soft debouncing
    if (millis() - lastTimePressed < 500)
    {
        return;
    }
    lastTimePressed = millis();
    Serial.print("button pressed ");
    Serial.print(millis());
    Serial.println(" milliseconds after restart");
}

/*
    @brief function for handling rotary current value and print into serial
*/
void rotary_loop()
{
    // dont print anything unless value changed
    if (rotaryEncoder.encoderChanged())
    {
        Serial.print("Value: ");
        Serial.println(rotaryEncoder.readEncoder());
    }
    if (rotaryEncoder.isEncoderButtonClicked())
    {
        rotary_onButtonClick();
    }
}

void IRAM_ATTR readEncoderISR()
{
    rotaryEncoder.readEncoder_ISR();
}
