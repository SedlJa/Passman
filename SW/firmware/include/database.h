#ifndef DATABASE_H
#define DATABASE_H

#include "encryption.h"
#include "esp_spiffs.h" // filesystem
#include <Arduino.h>
#include <WString.h>
#include <vector>

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

void parseAndStoreData(String data, int pos);
void readSerialData(String inputString, bool status, int pos);
void init_spiffs_db(void);
void db_add(int id, const char *username, const char *password);
void load_db_from_file(void);
void db_update(int target_id, const char *newusername, const char *newpassword);
void save_spiffs_db(void);
void delete_spiffs_db(void);

#endif // DATABASE_H