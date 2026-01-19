#ifndef DATABASE_H
#define DATABASE_H

#include "encryption.h"
#include <Arduino.h>
#include <WString.h>
#include <vector>

#define DB_LENGTH 10

using namespace std;

// Structure to represent a database
struct Database {
  String id[DB_LENGTH];
  String username[DB_LENGTH];
  String password[DB_LENGTH];
};

extern Database db;

void loadDatabase();
void parseAndStoreData(String data, int pos);
void readSerialData(String inputString, bool status, int pos);

#endif // DATABASE_H