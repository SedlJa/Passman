#include "oled.h"

/* Defines */
#define DELAYTIME 1000
#define DISPLAYI2C_ADDR 0x3C
#define DISPLAYWIDTH 128
#define DISPLAYHEIGH 64

/* Create diplay instance */
Adafruit_SH1106G display = Adafruit_SH1106G(DISPLAYWIDTH, DISPLAYHEIGH, &Wire, -1);

/* Functions */

/*
    @brief function that setups OLED display
*/
void display_setup()
{
    Wire.setPins(5, 4); // Set I2C pins [sda, scl ]
    Wire.begin();       // Inicializace I2C

    display.begin(DISPLAYI2C_ADDR, true);
    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);
    display.setTextSize(1);
}

void test_print(int rotaryVal)
{
    // put your main code here, to run repeatedly:
    display.setCursor(0, 7);
    display.println((String) "Teplota: 25C");
    display.println((String) "Tlak: 950hPa");
    display.println((String) "Vlhkost: 65%");
    display.println((String) "Loviskuju ZUZANKU");
    display.println((String) "Rotary value: " + (String)rotaryVal);
    display.display();
    delay(10);
    display.clearDisplay();
}

/*
    @brief function for displaying the first page
    @param rotaryVal current encoder value
*/
void menuPage1(int rotaryVal)
{
    // put your main code here, to run repeatedly:
    display.setCursor(0, 0);
    display.println((String) "-> Database");
    display.println((String) " ");
    display.println((String) "Password Generator");
    display.println((String) " ");
    display.println((String) "Credentials");
    display.println((String) " ");
    display.println((String) " ");
    display.println((String) "Rotary value: " + (String)rotaryVal);
    display.display();
    delay(10);
    display.clearDisplay();
}

/*
    @brief function for displaying the second page
    @param rotaryVal current encoder value
*/
void menuPage2(int rotaryVal)
{
    // put your main code here, to run repeatedly:
    display.setCursor(0, 0);
    display.println((String) "Database");
    display.println((String) " ");
    display.println((String) "-> Password Generator");
    display.println((String) " ");
    display.println((String) "Credentials");
    display.println((String) " ");
    display.println((String) " ");
    display.println((String) "Rotary value: " + (String)rotaryVal);
    display.display();
    delay(10);
    display.clearDisplay();
}

/*
    @brief function for displaying the third page
    @param rotaryVal current encoder value
*/
void menuPage3(int rotaryVal)
{
    // put your main code here, to run repeatedly:
    display.setCursor(0, 0);
    display.println((String) "Database");
    display.println((String) " ");
    display.println((String) "Password Generator");
    display.println((String) " ");
    display.println((String) "-> Credentials");
    display.println((String) " ");
    display.println((String) " ");
    display.println((String) "Rotary value: " + (String)rotaryVal);
    display.display();
    delay(10);
    display.clearDisplay();
}

/*
    @brief function for displaying the author info Page
*/
void authorInfo()
{
    // put your main code here, to run repeatedly:
    display.setCursor(0, 0);
    display.println((String) "bc. Jan Sedlak");
    display.println((String) "");
    display.println((String) "CTU in Prague");
    display.println((String) " ");
    display.println((String) "Faculty of electricalengeneering");
    display.println((String) " ");
    display.println((String) "Press to return...");
    display.display();
    delay(10);
    display.clearDisplay();
}
