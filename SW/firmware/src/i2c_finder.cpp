#include "i2c_finder.h"

/* OLED I2C address */
byte oled_addr = 0x3C;

/*
    @brief  Function for finding an OLED display I2C address
    @return 0 - OLED addres found
            1 - No I2C devices connected
            2 - Unknown error
*/
int find_I2C_addr()
{
    byte error_i2c, address_i2c; // promenne pro adresu a navrat chyby
    int I2C_Devices;             // promena pro pocet nalezenych zarizeni
    int retVal;
    Serial.println("Hledani zapocalo"); // vypis hlasky na Serial Monitor
    I2C_Devices = 0;                    // pocatecni stav nalezenych zarizeni je 0
    for (address_i2c = 1; address_i2c < 127; address_i2c++)
    {                                        // prochazi se adresy od 0 do 127
        Wire.beginTransmission(address_i2c); // pokus o prenos do zarizeni zadane adresy
        error_i2c = Wire.endTransmission();  // navrat stavove hodnoty I2C prenosu
        if (error_i2c == 0)
        {
            // pokud je navratova hodnota 0, je to OK, zarizeni existuje
            if (address_i2c < 16)
            { // pokud je adresa mensi nez 16, bude hex adresa jednociferna
                Serial.print("0");
            }
            if (address_i2c == oled_addr)
            {
                retVal = 0;
            }
            I2C_Devices++; // naslo se zarizeni, tak zvys počet o 1
        }
        else if (error_i2c == 4)
        {
            // zarizeni tam je, ale nastala nejaka chyba
            if (address_i2c < 16)
            { // opet reseni formatu vypisu adresy mensi nez 16
                Serial.print("0");
            }
            Serial.println(address_i2c, HEX);
            retVal = 2;
        }
    } // probehlo se všech 3 adres, tak se vypise zaver
    if (I2C_Devices == 0)
    { // nic se nenaslo
        retVal = 1;
    }
    return (retVal);
}