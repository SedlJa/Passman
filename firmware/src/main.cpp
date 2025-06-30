#include <Arduino.h>

/* Include of own libs */
#include "rotary.h"
#include "oled.h"
#include "authentication.h"

void setup()
{
  Serial.begin(115200);

  /* Rotary encoder init */
  rotary_setup();

  /* OLED display init */
  display_setup();
}

void loop()
{
  // PIN AUTHENTICATION
  // requires inserting a 4-digit pin.
  insertPin();

  // reset encoder
  rotaryEncoder.setBoundaries(1, 3);
  rotaryEncoder.setEncoderValue(1);

  // BASIC MENU
  if (rotaryEncoder.readEncoder() == 1)
  {
    menuPage1(rotaryEncoder.readEncoder());
  }
  else if (rotaryEncoder.readEncoder() == 2)
  {
    menuPage2(rotaryEncoder.readEncoder());
  }
  else if (rotaryEncoder.readEncoder() == 3)
  {
    menuPage3(rotaryEncoder.readEncoder());
    if (rotaryEncoder.isEncoderButtonClicked())
    {
      while (!rotaryEncoder.isEncoderButtonClicked())
      {
        authorInfo();
      }
    }
  }
  else
  {
    test_print(rotaryEncoder.readEncoder());
  }
}
