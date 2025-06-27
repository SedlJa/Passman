#include <Arduino.h>
#include "rotary.h"
#include "oled.h"

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
