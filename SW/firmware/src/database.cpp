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