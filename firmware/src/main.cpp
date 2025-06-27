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
  // put your main code here, to run repeatedly:
  rotary_loop();
  delay(20); // Add a small delay to avoid flooding the serial output

  if (rotaryEncoder.readEncoder() == 1)
  {
    page1(rotaryEncoder.readEncoder());
  }
  else if (rotaryEncoder.readEncoder() == 2)
  {
    page2(rotaryEncoder.readEncoder());
  }
  else
  {
    test_print(rotaryEncoder.readEncoder());
  }
}
