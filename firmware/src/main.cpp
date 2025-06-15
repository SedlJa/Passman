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
  delay(50); // Add a small delay to avoid flooding the serial output

  // display test print
  test_print(rotaryEncoder.readEncoder());
}
