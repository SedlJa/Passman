#include <database.h>

Database db;

/**
 * @brief function for loading database content
 */
void loadDatabase()
{
    /* Load IDs */
    for (int i = 0; i < DB_LENGTH; i++)
    {
        /* code */
        db.id[i] = i + 1;
    }
    db.username[0] = "uzivatel 1";
    db.password[0] = "superHesl0";
}

/**
 * @brief function to handle incomming serial port communication
 */
void parseAndStoreData(String data, int pos)
{
    int firstDelimiter = data.indexOf(';');
    int secondDelimiter = data.lastIndexOf(';');

    // Basic check to ensure we have at least two delimiters
    if (firstDelimiter == -1 || secondDelimiter == -1 || firstDelimiter == secondDelimiter)
    {
        Serial.println("Error: Invalid data format.");
        return;
    }

    // Extract ID
    db.id[pos] = data.substring(0, firstDelimiter);
    // Extract Username
    db.username[pos] = data.substring(firstDelimiter + 1, secondDelimiter);
    // Extract Password
    db.password[pos] = data.substring(secondDelimiter + 1);
}

void readSerialData(String receivedData, bool status, int pos)
{
    while (!status)
    {
        if (Serial.available())
        {
            char inChar = (char)Serial.read();
            receivedData += inChar;
            if (inChar == '\n')
            {
                status = true;
            }
        }
    }
    receivedData.trim(); // Remove any trailing newline or spaces
    parseAndStoreData(receivedData, pos);
    receivedData = "";
    status = false;
}