#define DATABASE_H

#include <WString.h>
#include <vector>
#include <Arduino.h>
#include "encryption.h"

#define DB_LENGTH 10

using namespace std;

// Structure to represent a database
struct Database
{
    String id[DB_LENGTH];
    String username[DB_LENGTH];
    String password[DB_LENGTH];
};

extern Database db;

void loadDatabase();
void parseAndStoreData(String data, int pos);
void readSerialData(String inputString, bool status, int pos);