#define DATABASE_H

#include <WString.h>
#include <vector>

#define DB_LENGTH 10

using namespace std;

// Structure to represent a database
struct Database
{
    int id[DB_LENGTH];
    String username[DB_LENGTH];
    String password[DB_LENGTH];
};

extern Database db;

void loadDatabase();